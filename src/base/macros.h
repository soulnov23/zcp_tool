#pragma once

// class uncopyable
#define CLASS_UNCOPYABLE(class_name)        \
private:                                    \
    class_name(const class_name&) = delete; \
    class_name& operator=(const class_name&) = delete;

// class unmovable
#define CLASS_UNMOVABLE(class_name)          \
private:                                     \
    class_name(const class_name&&) = delete; \
    class_name& operator=(const class_name&&) = delete;
