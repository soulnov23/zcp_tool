#ifndef __ARG_H__
#define __ARG_H__

class arg {
 public:
  arg() {}
  virtual ~arg() {}

  virtual void free() = 0;
};

#endif