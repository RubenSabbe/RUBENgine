#pragma once
//Source https://discuss.cocos2d-x.org/t/correct-way-to-implement-singleton/28226/26
#ifndef __SINGLETON_H__
#define __SINGLETON_H__
namespace RUBENgine
{
    template <typename T>
    class Singleton
    {
    public:
        inline static T* GetInstance();
        inline static void Release();
    private:
        static T* t;
    };

    template <typename T>
    inline T* Singleton<T>::GetInstance()
    {
        if (!t)
        {
            t = new T;
        }
        return t;
    }

    template<typename T>
    inline void Singleton<T>::Release()
    {
        if (t)
        {
            delete t;
            t = 0;
        }
    }

    template <typename T>
    T* Singleton<T>::t = 0;
}
#endif // __SINGLE_H__