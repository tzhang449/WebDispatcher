#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class Noncopyable
{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;

private:
    Noncopyable(const Noncopyable &) = delete;
    const Noncopyable &opeartor = (const Noncopyable &) = delete;
};

#endif