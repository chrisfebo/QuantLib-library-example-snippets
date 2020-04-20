/* FX OPTION PRICING */

/* This example sets up an FX option with European exercise. The rates of the 2 currencies
 * are modeled by creating yield curve term structures. The stochastic process used for
 * modeling the behaviour of the currency pair is Garman-Kohlagen. The FX put option is priced
 * using an integral method for vanilla options. */

#include <ql/qldefines.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/pricingengines/vanilla/integralengine.hpp>
#include <ql/exercise.hpp>
#include <iostream>

using namespace QuantLib;

int main (int, char* []) {

	// date and calendar settings
	Calendar calendar = TARGET();
	Date todaysDate (25, February, 2019);
	Date settlementDate = todaysDate + Period(2, Days);
	Date maturityDate = settlementDate + Period(6, Months);
    DayCounter dayCounter = Actual365Fixed();
    Settings::instance().evaluationDate() = todaysDate;

    // market data and option parameters
    Option::Type optionType (Option::Put);
    Exercise::Type exerciseType (Exercise::European);

    Real contractSize = 100;
    Real strike = 110;

    Rate domesticRate = 0.0268; // taken from 10y US gov bonds
    Rate foreignRate = 0.0315; // taken from 10y CN gov bonds

    Volatility volatility = 0.06;


    // print information
    std::cout << "Today = "      << todaysDate << std::endl;
    std::cout << "Option type = "  << optionType << std::endl;
    std::cout << "Exercise type = " << exerciseType << std::endl;
    std::cout << "Maturity = "        << maturityDate << std::endl;
    std::cout << "Underlying price = "        << contractSize << std::endl;
    std::cout << "Strike = "                  << strike << std::endl;
    std::cout << "Risk-free interest rate in US = " << domesticRate
              << std::endl;
    std::cout << "Risk-free interest rate in CN = " << foreignRate
              << std::endl;
    std::cout << "Volatility = " << volatility
              << std::endl;
    std::cout << std::endl;

	// create quotes and term structures for yield, volatility
    Handle<Quote> spot(ext::shared_ptr<Quote> (new SimpleQuote(contractSize)));

    Handle<YieldTermStructure> domesticYield(ext::shared_ptr<YieldTermStructure> (
    		new FlatForward(settlementDate, domesticRate, dayCounter)));
    Handle<YieldTermStructure> foreignYield(ext::shared_ptr<YieldTermStructure> (
    		new FlatForward(settlementDate, foreignRate, dayCounter)));

    Handle<BlackVolTermStructure> volatilityTermStructure(ext::shared_ptr<BlackVolTermStructure> (
    		new BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));

    // create exercise, payoff, option objects,
    ext::shared_ptr<Exercise> exercise(new EuropeanExercise(maturityDate));
    ext::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(optionType,strike));
	VanillaOption theOption(payoff, exercise);

	//create Garman-Kholagen stochastic process and Integral pricing engine
	ext::shared_ptr<GarmanKohlagenProcess> gkProcess(
			new GarmanKohlagenProcess(spot, foreignYield, domesticYield, volatilityTermStructure));
	ext::shared_ptr<PricingEngine> integralEngine(new IntegralEngine(gkProcess));

	// price the option and print result
	theOption.setPricingEngine(integralEngine);
	Real price = theOption.NPV();
	std::cout<< "Price = " << price << std::endl;

    return 0;
}






