#ifndef CLIENT_ASSEMBLINGCHOOSER_HPP
#define CLIENT_ASSEMBLINGCHOOSER_HPP

#include <IChooser.hpp>

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
        std::unique_ptr<IChooser> chooser = chooserFactory(); // TODO: give parameters from potentialSteps
        return chooser->chooseGoodStep(potentialSteps);
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        std::unique_ptr<IChooser> chooser = chooserFactory(); // TODO: give parameters from potentialSteps
        return chooser->chooseBadStep(potentialSteps);
    }

private:
    ChooserFactory chooserFactory;
};

//============================================================================//

#endif // CLIENT_ASSEMBLINGCHOOSER_HPP
