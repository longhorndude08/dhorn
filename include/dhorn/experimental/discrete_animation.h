/*
 * Duncan Horn
 *
 * discrete_animation.h
 *
 * Updates values at discrete times. I.e. if there are two key frames (0, 0) and (1, 1), then any time between time
 * [0, 1) we will apply the value 0 and any time after that interval we will apply the value 1. Note that for any key
 * frame, only one change will get sent.
 */
#pragma once

#include "animation.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * discrete_animation
         */
        template <typename Ty>
        class discrete_animation final :
            public details::key_frame_animation<Ty>
        {
            using MyBase = details::key_frame_animation<Ty>;

        public:
            /*
             * Constructor(s)/Destructor
             */
            discrete_animation(void) :
                _prev(this->next())
            {
            }

            discrete_animation(typename MyBase::update_function func) :
                MyBase(std::move(func)),
                _prev(this->next())
            {
            }



            /*
             * Overloaded animation functions
             */
            virtual animation_state on_update(typename MyBase::duration elapsedTime)
            {
                auto state = MyBase::on_update(elapsedTime);

                // Ignore any updates until the animation has started
                if (this->begun())
                {
                    // Only update if our next iterator changes
                    auto next = this->next();
                    --next;

                    if (next != this->_prev)
                    {
                        this->_prev = next;
                        this->update(next->second);
                    }
                }

                return state;
            }



        private:

            // NOTE: _prev will be an end iterator up until the animation begins
            typename MyBase::iterator_type _prev;
        };
    }
}
