#ifndef DE_H
#define DE_H

#include "Optimizer.hpp"
#include <vector>
#include <random>
#include <iosfwd>

template <typename T>
class DE : public Optimizer {
  public:
    struct Individual {
        std::vector<T> position;
        T score;
        typename Optimizer::Stats<T> stats;
    };

    explicit DE(int, int, T, T, std::vector<T>, std::vector<T>);
    virtual ~DE();

    const std::vector<Individual> &getPopulation();
    const std::vector<Individual> &getMutants();
    void initPopulation(const std::vector<T> &, const std::vector<T> &);
    void setEvalData(const int &, const std::vector<std::pair<T, T>> &);
    typename Optimizer::Stats<T> calcScore(const int &);
    void mutation();
    void selection();

    /** トレース出力（開発者用設定でON時のみ有効） */
    void setTraceStream(std::ostream* s) { traceStream_ = s; }
    void setTraceEnabled(bool en) { traceEnabled_ = en; }
    void writeTraceLine(int iteration) const;

  private:
    std::vector<Individual> population_; // 個体集団
    std::vector<Individual> mutants_;  // 変異個体集団
    int num_dim_;
    T F_; // 変異係数
    T CR_; // 交叉率
    std::vector<T> ul_bound_;
    std::vector<T> ll_bound_;
    std::mt19937 rng_;
    std::ostream* traceStream_ = nullptr;
    bool traceEnabled_ = false;
};

#endif // DE_H