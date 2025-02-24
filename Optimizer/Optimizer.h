#ifndef D_OPTIMIZER_H
#define D_OPTIMIZER_H

#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
//  Optimizer is responsible for ...
//
///////////////////////////////////////////////////////////////////////////////
// 
class Optimizer {
  public:
    explicit Optimizer();
    virtual ~Optimizer();

    struct Stats {
        double mean;
        double std_dev;
        double rmse;

        void initialize() {
            mean    = 0.0;
            std_dev = 0.0;
            rmse    = 0.0;
        }
    };
    Stats computeStats(const std::vector<std::pair<double, double>>&);
    virtual void setParam(const std::vector<double> &) {}

  private:
    Optimizer(const Optimizer &)            = delete;
    Optimizer &operator=(const Optimizer &) = delete;
};



#endif // D_OPTIMIZER_H
