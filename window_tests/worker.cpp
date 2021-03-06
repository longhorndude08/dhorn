/*
 * Duncan Horn
 *
 * worker.cpp
 */

#include "precomp.h"

#include "globals.h"

using namespace dhorn::experimental::win32;

worker::worker(void) :
    _topLeft(-2, 1),
    _bottomRight(1, -1),
    _nextRow(0),
    _iterations(0),
    _iterationsPerUpdate(1),
    _threadCount(0),
    _threadsExecuting(0),
    _sizeUpdatePending(false),
    _running(false)
{
    // For now, we will spawn one worker thread for each logical processor. The thought is that the UI thread shouldn't
    // have all that much work to do. This can be tweaked later
    this->_threadCount = get_maximum_processor_count();
}

worker::~worker(void)
{
}

void worker::start(void)
{
    // Register callback handlers
    globals::window.add_callback_handler(window_message::paint, dhorn::experimental::bind_member_function(&worker::on_paint, this));
    globals::window.add_callback_handler(window_message::size, dhorn::experimental::bind_member_function(&worker::on_resize, this));
    globals::window.add_callback_handler(
        window_message::erase_background,
        dhorn::experimental::bind_member_function(&worker::on_erase_background, this));
    globals::window.add_callback_handler(
        window_message::mouse_wheel,
        dhorn::experimental::bind_member_function(&worker::on_scrollwheel, this));

    // Initialize the number of threads executing since the update function assums that the thread was already
    // contributing to the execution count
    this->_threadsExecuting = this->_threadCount;

    this->dc = GetDC(globals::window.handle());
    this->update_size();

    // Finally, begin execution!
    this->_running = true;
    for (std::uint32_t i = 0; i < this->_threadCount; ++i)
    {
        this->_threads.emplace_back(dhorn::experimental::bind_member_function(&worker::thread_proc, this));
    }
}

void worker::exit(void)
{
    // Must join all threads since it is an error to let them destroy without being detached if they have not yet
    // completed execution
    for (auto &thread : this->_threads)
    {
        thread.join();
    }
    this->_threads.clear();
}

COLORREF DecideColor(std::size_t iterations)
{
    if (iterations)
    {
        static const std::size_t RESOLUTION = 50;
        static const float PI = 3.14159f;
        float angle = (iterations * 2 * PI) / RESOLUTION;
        float r = cos(angle);
        float g = cos(angle + (2 * PI / 3));
        float b = cos(angle + (4 * PI / 3));

        return RGB((UINT)(127 * (1 + r)), (UINT)(127 * (1 + g)), (UINT)(127 * (1 + b)));
    }

    return RGB(0, 0, 0);
}

void worker::thread_proc(void)
{
    while (this->_running)
    {
        int row_index = ++this->_nextRow;
        for (; row_index < static_cast<int>(this->_data->size()); row_index = ++this->_nextRow)
        {
            auto &row = (*this->_data)[row_index];
            for (int col_index = 0; col_index < static_cast<int>(row.size()); ++col_index)
            {
                auto &entry = row[col_index];

                // Skip points that are already complete
                if (entry.iterations)
                {
                    continue;
                }

                for (std::size_t i = 0; i < this->_iterationsPerUpdate; ++i)
                {
                    entry.value = (entry.value * entry.value) + entry.point;
                    if (dhorn::experimental::length_squared(entry.value.imag(), entry.value.real()) >= 4)
                    {
                        entry.iterations = this->_iterations + i;
                        entry.color = DecideColor(entry.iterations);

                        {
                            auto lock = std::unique_lock<std::mutex>(this->_drawMutex);
                            SetPixel(this->dc, col_index, row_index, entry.color);
                        }

                        break;
                    }
                }
            }

            // Allow response to size update to occur earlier
            if (this->_sizeUpdatePending)
            {
                break;
            }
        }

        this->synchronize_update();
    }
}

void worker::synchronize_update(void)
{
    std::unique_lock<std::mutex> lock(this->_monitor);
    --this->_threadsExecuting;

    // If we are the last thread to begin waiting, then it is our responsibility to notify the update thread that
    // it can proceed. We also use this opportunity to reset the next row
    if (this->_threadsExecuting == 0)
    {
        // Check to see if there was a size change
        if (this->_sizeUpdatePending)
        {
            this->update_size();
            this->_sizeUpdatePending = false;
        }

        // Reset state for the next iteration
        this->_nextRow = 0;
        this->_iterations += this->_iterationsPerUpdate;
        this->_updateReady.notify_all();
        this->_running = globals::window.running();

        // Update the UI thread and inform it to re-paint
        // TODO: save data
        if (this->_running)
        {
            globals::window.invalidate();
        }
    }
    else
    {
        // Wait for the size update to complete
        this->_updateReady.wait(lock);
    }

    ++this->_threadsExecuting;

    // Barrier until we know that all threads have reached this point
    if (this->_threadsExecuting != this->_threadCount)
    {
        this->_updateCompleted.wait(lock);
    }
    else
    {
        this->_updateCompleted.notify_all();
    }
}

void worker::update_size(void)
{
    auto size = globals::window.client_rect();
    this->_iterations = 0;

    // We treat the indices as (row, column), so (x, y) is flipped
    this->_data = std::make_shared<DataType>(size.height);
    for (std::size_t i = 0; i < size.height; ++i)
    {
        auto &row = (*this->_data)[i];
        long double deltaY = (this->_topLeft.imag() - this->_bottomRight.imag()) * i / size.height;
        for (std::size_t j = 0; j < size.width; ++j)
        {
            long double deltaX = (this->_bottomRight.real() - this->_topLeft.real()) * j / size.width;
            ComplexType pt(this->_topLeft.real() + deltaX, this->_topLeft.imag() - deltaY);
            row.push_back(pt);
        }
    }

    // Fill with black
    {
        auto lock = std::unique_lock<std::mutex>(this->_drawMutex);
        RECT rc = { 0, 0, (LONG)size.width, (LONG)size.height };
        dhorn::experimental::unique_brush black = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(this->dc, &rc, black);
    }

}

callback_handler::result_type worker::on_paint(window *pWindow, std::uintptr_t /*wparam*/, std::intptr_t /*lparam*/)
{
    (void)pWindow;
    //auto ps = pWindow->begin_paint();

    //// Grab a strong reference to the data (in case the size changes)
    //auto data = this->_data;
    //auto width = (*data)[0].size();
    //auto height = data->size();

    //// Create the bitmap that we will draw to
    //dhorn::unique_deletable_dc dc = CreateCompatibleDC(ps.device_context);
    //dhorn::unique_bitmap backBuffer = CreateCompatibleBitmap(ps.device_context, width, height);

    //int savedDC = SaveDC(dc);
    //SelectObject(dc, backBuffer);

    //// For now, we only use black and white
    //dhorn::unique_brush white = CreateSolidBrush(RGB(255, 255, 255));

    //RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    //FillRect(dc, &rc, white);

    //// Fill in the data
    //for (std::size_t i = 0; i < data->size(); ++i)
    //{
    //    auto &list = (*data)[i];
    //    for (std::size_t j = 0; j < list.size(); ++j)
    //    {
    //        auto &val = list[j];
    //        SetPixel(dc, j, i, DecideColor(val.color));
    //    }
    //}

    //BitBlt(ps.device_context, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    //RestoreDC(dc, savedDC);

    return std::make_pair(false, 0);
}

callback_handler::result_type worker::on_resize(
    window * /*pWindow*/,
    std::uintptr_t /*wparam*/,
    std::intptr_t /*lparam*/)
{
    this->_sizeUpdatePending = true;
    return std::make_pair(true, 0);
}

callback_handler::result_type worker::on_erase_background(
    window * /*pWindow*/,
    std::uintptr_t /*wparam*/,
    std::intptr_t /*lparam*/)
{
    // Return true so that the background will not get cleared
    return std::make_pair(true, 1);
}

callback_handler::result_type worker::on_scrollwheel(
    window * /*pWindow*/,
    std::uintptr_t wparam,
    std::intptr_t lparam)
{
    POINT pt = { LOWORD(lparam), HIWORD(lparam) };
    ScreenToClient(globals::window.handle(), &pt);

    // Ignore if not in the client area
    auto data = this->_data;
    dhorn::experimental::rect<std::size_t> size = { (*data)[0].size(), data->size() };
    if ((pt.x < 0) || (pt.x >= (LONG)size.width) || (pt.y < 0) || (pt.y >= (LONG)size.height))
    {
        return std::make_pair(false, 0);
    }

    // Percentage of the current window
    auto scrollAmt = static_cast<std::int16_t>(HIWORD(wparam)) / 120.0f;
    float amt = 4. / (scrollAmt * 3 + 5);

    auto new_width = static_cast<std::int64_t>(size.width * amt);
    auto new_height = static_cast<std::int64_t>(size.height * amt);

    auto left = pt.x - new_width / 2;
    auto top = pt.y - new_height / 2;
    auto right = left + new_width;
    auto bottom = top + new_height;

    // Convert these co-ords to complex values
    auto topLeftReal = this->_topLeft.real() + (this->_bottomRight.real() - this->_topLeft.real()) * left / size.width;
    auto topLeftImag = this->_topLeft.imag() - (this->_topLeft.imag() - this->_bottomRight.imag()) * top / size.height;
    auto bottomRightReal = this->_topLeft.real() + (this->_bottomRight.real() - this->_topLeft.real()) * right / size.width;
    auto bottomRightImag = this->_topLeft.imag() - (this->_topLeft.imag() - this->_bottomRight.imag()) * bottom / size.height;

    this->_topLeft = { topLeftReal, topLeftImag };
    this->_bottomRight = { bottomRightReal, bottomRightImag };

    // Schedule a resize since we need to clear our buffers
    this->_sizeUpdatePending = true;
    return std::make_pair(true, 0);
}
