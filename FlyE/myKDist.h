#include <random>

class myDist {
 public:
 virtual int operator() (std::mt19937 &generator) = 0;

 virtual ~myDist() {}
};

class myUniformDist : public myDist {
 private:
  std::uniform_int_distribution<> dist_;

 public:
  myUniformDist(int kLower, int kUpper) {
    std::uniform_int_distribution<>::param_type params{kLower, kUpper};
    dist_.param(params);
  }

  int operator() (std::mt19937 &generator) {
    return dist_(generator);
  }
};

class mySingleDist : public myDist {
 private:
  int k_;

 public:
  mySingleDist(int k) : k_(k) {}

  int operator() (std::mt19937 &generator) {
    return k_;
  }
};

class myTriangleDist : public myDist {
 private:
  std::piecewise_linear_distribution<> dist_;

 public:
  myTriangleDist(std::vector<float> &kpieces, std::vector<float> &kweights) {
    std::piecewise_linear_distribution<>::param_type params{kpieces.begin(), kpieces.end(), kweights.begin()};
    dist_.param(params);
  }

  int operator() (std::mt19937 &generator) {
    return (int) dist_(generator);
  }
};
