#ifndef D_OPTIMIZER_H
#define D_OPTIMIZER_H

#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector> 

/////////////////////////////////////////////////////////////////////////////
//
//  Handler is responsible for ...
//
/////////////////////////////////////////////////////////////////////////////

template <typename T>
class Handler {
  public:
    explicit Handler(const string&);
    virtual ~Handler();

  private:
    string algorighm_name_;
    string file_name_;
    void coilPreProcess();
    void coilPostProcess();
    void segmentPreProcess();
    void segmentPostProcess();
    void paramPreProcess();
    void paramPostProcess();


};

#endif // D_OPTIMIZER_H