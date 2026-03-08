#include "CppUTest/TestHarness.h"
#include "objective/Objective.hpp"
#include "param/param.hpp"
#include "product/BatchEvaluationHandler.hpp"
#include "product/ProductRunner.hpp"
#include "product/ProductMeta.hpp"
#include "mock/Mock.hpp"
#include <vector>

TEST_GROUP(Objective) {};

TEST(Objective, EvaluateReturnsResidualsAndObjective) {
    std::vector<optimizer::ParamSpec> specs = {
        {"p1", 1, optimizer::InitMode::Manual, 0.0, "", -10.0, 10.0, ""},
    };
    optimizer::ParameterMapper mapper;
    mapper.setSpecs(specs);
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::BatchEvaluationHandler batch(runner);
    batch.setProducts({{"X", "x"}});
    optimizer::Objective obj(mapper, batch);
    std::vector<double> x = {0.5};
    auto res = obj.evaluate(x);
    CHECK(res.residuals.size() > 0);
    CHECK(res.objective >= 0.0);
}

TEST(Objective, EvaluateWithJacobianReturnsJacobian) {
    std::vector<optimizer::ParamSpec> specs = {
        {"p1", 1, optimizer::InitMode::Manual, 0.0, "", -10.0, 10.0, ""},
        {"p2", 1, optimizer::InitMode::Manual, 0.0, "", -10.0, 10.0, ""},
    };
    optimizer::ParameterMapper mapper;
    mapper.setSpecs(specs);
    optimizer::MockPhysicalModel model;
    optimizer::MockProductDataLoader loader;
    optimizer::ProductRunner runner(model, loader);
    optimizer::BatchEvaluationHandler batch(runner);
    batch.setProducts({{"X", "x"}});
    optimizer::Objective obj(mapper, batch);
    std::vector<double> x = {0.0, 0.0};
    auto jr = obj.evaluateWithJacobian(x);
    LONGS_EQUAL(jr.residuals.size(), jr.jacobian.size());
    if (!jr.jacobian.empty())
        LONGS_EQUAL(2, jr.jacobian[0].size());
}
