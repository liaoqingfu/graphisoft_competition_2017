#include "Options.hpp"

#include <boost/program_options.hpp>

#include <iostream>

namespace {

namespace po = boost::program_options;

template <typename T>
po::typed_value<T>* defaultValue(T& v) {
    return po::value(&v)->default_value(v);
}

} // unnamed namespace

Options parseOptions(int argc, const char* argv[]) {
    po::options_description optionsDescription;
    Options options;
    bool help = false;
    optionsDescription.add_options()
        ("help,h", po::bool_switch(&help))
        ("num-runs,n", defaultValue(options.numRuns))
        ("seed", defaultValue(options.seed))
        ("strategy,s", po::value(&options.strategyStrings)->multitoken())
        ("width,w", defaultValue(options.width))
        ("height,h", defaultValue(options.height))
        ("num-displays,m", defaultValue(options.numDisplays))
        ("time-limit,t", defaultValue(options.maxTick))
        ("jobs,j", defaultValue(options.jobs))
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(optionsDescription).
            run(), vm);
    po::notify(vm);

    if (help) {
        std::cerr << optionsDescription << "\n";
        exit(0);
    }
    return options;
}

