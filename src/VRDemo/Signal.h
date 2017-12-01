#ifndef SIGNAL_H
#define SIGNAL_H
#include <map>
#include <functional>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    /**
     * \brief Basic signal class. Allows you to write observer patterns while avoiding boilerplate code.
     * \tparam Args Parameter pack. The arguments you want to pass to the slots when calling Notify.
     */
    template <typename... Args>
    class Signal
    {
    public:
        Signal()
            : id(0)
        {}

        template <typename T>
        int Connect(T *inst, void (T::*func)(Args...))
        {
            return Connect([=](Args... args)
            {
                (inst->*func)(args...);
            });
        }

        template <typename T>
        int Connect(T *inst, void (T::*func)(Args...) const)
        {
            return Connect([=](Args... args)
            {
                (inst->*func)(args...);
            });
        }

        int Connect(const std::function<void(Args...)>& slot) const
        {
            slots.insert(std::make_pair(++id, slot));
            return id;
        }

        void Disconnect(int id) const
        {
            slots.erase(id);
        }

        void Clear() const
        {
            slots.clear();
        }

        Signal operator()(Args... args) const
        {
            Notify(args...);
            return *this;
        }
    private:
        mutable std::map<int, std::function<void(Args...)>> slots;
        mutable int id;

        void Notify(Args... args) const
        {
            for (auto it : slots)
            {
                it.second(args...);
            }
        }
    };
}

#endif // SIGNAL_H
