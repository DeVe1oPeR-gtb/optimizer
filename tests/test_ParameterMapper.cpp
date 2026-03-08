#include "CppUTest/TestHarness.h"
#include "param/ParameterMapper.hpp"
#include "param/CsvParamLoader.hpp"
#include "param/ParamSpec.hpp"
#include <vector>

TEST_GROUP(ParameterMapper) {};

TEST(ParameterMapper, EnableOpt) {
    std::vector<optimizer::ParamSpec> specs = {
        {"a", 1, optimizer::InitMode::Manual, 0.5, "", 0.0, 1.0, ""},
        {"b", 0, optimizer::InitMode::Manual, 1.0, "", 0.0, 0.0, ""},
        {"c", 1, optimizer::InitMode::Manual, -0.2, "", -1.0, 1.0, ""},
    };
    optimizer::ParameterMapper pm;
    pm.setSpecs(specs);
    LONGS_EQUAL(2, pm.numOptParams());
    LONGS_EQUAL(3, pm.numFullParams());
    auto lo = pm.getLowerBounds();
    auto up = pm.getUpperBounds();
    auto ab = pm.getApplyBounds();
    LONGS_EQUAL(2, lo.size());
    DOUBLES_EQUAL(0.0, lo[0], 1e-12);
    DOUBLES_EQUAL(-1.0, lo[1], 1e-12);
    DOUBLES_EQUAL(1.0, up[0], 1e-12);
    DOUBLES_EQUAL(1.0, up[1], 1e-12);
    LONGS_EQUAL(2, ab.size());
    CHECK(ab[0]);
    CHECK(ab[1]);
}

TEST(ParameterMapper, InitModeManual) {
    std::vector<optimizer::ParamSpec> specs = {
        {"x", 1, optimizer::InitMode::Manual, 0.7, "", 0.0, 1.0, ""},
    };
    optimizer::ParameterMapper pm;
    pm.setSpecs(specs);
    auto x0 = pm.getInitialVector(nullptr);
    LONGS_EQUAL(1, x0.size());
    DOUBLES_EQUAL(0.7, x0[0], 1e-12);
}

TEST(ParameterMapper, ExpandAndBack) {
    std::vector<optimizer::ParamSpec> specs = {
        {"a", 1, optimizer::InitMode::Manual, 1.0, "", 0, 2, ""},
        {"b", 0, optimizer::InitMode::Manual, 2.0, "", 0, 0, ""},
        {"c", 1, optimizer::InitMode::Manual, 3.0, "", 0, 10, ""},
    };
    optimizer::ParameterMapper pm;
    pm.setSpecs(specs);
    std::vector<double> x_opt = {1.5, 4.0};
    auto full = pm.expandToFullParameterSet(x_opt);
    LONGS_EQUAL(3, full.size());
    DOUBLES_EQUAL(1.5, full[0], 1e-12);
    DOUBLES_EQUAL(2.0, full[1], 1e-12);
    DOUBLES_EQUAL(4.0, full[2], 1e-12);
    auto back = pm.fullToOptVector(full);
    DOUBLES_EQUAL(1.5, back[0], 1e-12);
    DOUBLES_EQUAL(4.0, back[1], 1e-12);
}

TEST(ParameterMapper, CsvLoadAndValidate) {
    optimizer::CsvParamLoader loader;
    std::string err;
    auto specs = loader.load("config/params_sample.csv", err);
    CHECK(err.empty());
    CHECK(specs.size() >= 2);
    optimizer::ParameterMapper pm;
    pm.setSpecs(specs);
    std::string verr;
    CHECK(pm.validate(verr));
}

TEST(ParameterMapper, ValidateDuplicateFails) {
    std::vector<optimizer::ParamSpec> specs = {
        {"a", 1, optimizer::InitMode::Manual, 0.5, "", 0.0, 1.0, ""},
        {"a", 0, optimizer::InitMode::Manual, 0.0, "", 0.0, 0.0, ""},
    };
    optimizer::CsvParamLoader loader;
    std::string err;
    CHECK_FALSE(loader.validate(specs, {}, err));
    CHECK(err.find("Duplicate") != std::string::npos);
}
