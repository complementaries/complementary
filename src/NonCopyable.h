#ifndef NON_COPYABLE_H
#define NON_COPYABLE_H

class NonCopyable {
  public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

  protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

#endif
