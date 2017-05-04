#ifndef CLIENT_ASSEMBLINGCHOOSER_HPP
#define CLIENT_ASSEMBLINGCHOOSER_HPP

#include "IChooser.hpp"

#include <cassert>
#include <memory>

//============================================================================//

template<typename ChooserFactory>
class AssemblingChooser : public IChooser {
public:
    explicit AssemblingChooser(ChooserFactory chooserFactory)
            : chooserFactory(chooserFactory) {
    }

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        assert(potentialSteps.size() > 0);
        std::unique_ptr<IChooser> chooser = chooserFactory(
                potentialSteps.back().getGameState());
        return chooser->chooseGoodStep(potentialSteps);
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        assert(potentialSteps.size() > 0);
        std::unique_ptr<IChooser> chooser = chooserFactory(
                potentialSteps.back().getGameState());
        return chooser->chooseBadStep(potentialSteps);
    }

private:
    ChooserFactory chooserFactory;
};

//============================================================================//

#endif // CLIENT_ASSEMBLINGCHOOSER_HPP
