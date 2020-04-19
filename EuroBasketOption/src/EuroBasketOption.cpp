/* EUROPEAN BASKET OPTION PRICING */

/* This example sets up a a European option on a basket of 2 underlying equities.
 * the option is priced using a Monte Carlo pricing engine. The underlyings are simulated
 * using a 2-dimensional Black-Scholes-Merton process taking into account correlation.
 * The output is the price of the option given the initial conditions. */

#include <ql/qldefines.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/instruments/basketoption.hpp>
#include <ql/pricingengines/basket/mceuropeanbasketengine.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <iostream>

using namespace QuantLib;

int main(int, char* []) {

	// set dates
	Calendar calendar = TARGET();
	Date todaysDate (20, February, 2019);
	Date settlementDate (22, February, 2019);
    Date maturity(22, February, 2020);
    DayCounter dayCounter = Actual365Fixed();
    Settings::instance().evaluationDate() = settlementDate;

    //define market data, underlying securities and option payoff parameters
    Option::Type type(Option::Call);
    Rate riskFreeRate = 0.05;

    Real underlying1 = 100;
    Spread dividendYield1 = 0.00;
    Volatility volatility1 = 0.30;

    Real underlying2 =100;
    Spread dividendYield2 = 0.00;
    Volatility volatility2 = 0.30;

    Real strike = 100;

    std::cout << "Option type = "  << type << std::endl;
    std::cout << "Maturity = "        << maturity << std::endl;
    std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate)
              << std::endl;
    std::cout << "Underlying price of stock 1 = "        << underlying1 << std::endl;
    std::cout << "Dividend yield of stock 1 = " << io::rate(dividendYield1)
              << std::endl;
    std::cout << "Volatility of stock 1 = " << io::volatility(volatility1)
              << std::endl;
    std::cout << "Underlying price of stock 2 = "        << underlying2 << std::endl;
    std::cout << "Dividend yield of stock 2 = " << io::rate(dividendYield2)
              << std::endl;
    std::cout << "Volatility of stock 2 = " << io::volatility(volatility2)
              << std::endl;
    std::cout << "Strike = " << strike << std::endl;
    std::cout << std::endl;
    std::string method;
    std::cout << std::endl;

    // write column headings for results
    Size widths[] = { 35, 35 };
    std::cout << std::setw(widths[0]) << std::left << "Method"
              << std::setw(widths[1]) << std::left << "Price"
			  << std::endl;

	// create quotes and curves (yield, dividend, volatility) for each of the 2 underlyings
    Handle<Quote> spot1(ext::shared_ptr<Quote> (new SimpleQuote(underlying1)));
    Handle<Quote> spot2(ext::shared_ptr<Quote> (new SimpleQuote(underlying2)));

    Handle<YieldTermStructure> yieldCurve(ext::shared_ptr<YieldTermStructure> (
    		new FlatForward(settlementDate, riskFreeRate, dayCounter)));

    Handle<YieldTermStructure> dividendCurve1(ext::shared_ptr<YieldTermStructure> (
    		new FlatForward(settlementDate, dividendYield1, dayCounter)));
    Handle<YieldTermStructure> dividendCurve2(ext::shared_ptr<YieldTermStructure> (
    		new FlatForward(settlementDate, dividendYield2, dayCounter)));

    Handle<BlackVolTermStructure> volatilityCurve1(ext::shared_ptr<BlackVolTermStructure> (
    		new BlackConstantVol(settlementDate, calendar, volatility1, dayCounter)));
    Handle<BlackVolTermStructure> volatilityCurve2(ext::shared_ptr<BlackVolTermStructure> (
    		new BlackConstantVol(settlementDate, calendar, volatility2, dayCounter)));

	// create exercise, payoff, and option
	ext::shared_ptr<Exercise> exercise(new EuropeanExercise(maturity));
    ext::shared_ptr<StrikedTypePayoff> plainPayoff(new PlainVanillaPayoff(type,strike));

    ext::shared_ptr<BasketPayoff> payoff(new MinBasketPayoff(plainPayoff));
    BasketOption basketOption(payoff, exercise);

    //create stochastic process to simulate the underlyings
    ext::shared_ptr<BlackScholesMertonProcess> process1 (
    		new BlackScholesMertonProcess(spot1, dividendCurve1, yieldCurve, volatilityCurve1));
    ext::shared_ptr<BlackScholesMertonProcess> process2 (
    		new BlackScholesMertonProcess(spot2, dividendCurve2, yieldCurve, volatilityCurve2));

    std::vector<ext::shared_ptr<StochasticProcess1D>> processes;
    processes.push_back(process1);
    processes.push_back(process2);

    Matrix correlationMatrix(2,2, 1.0);
    correlationMatrix[0][1] = 0.50;
    correlationMatrix[1][0] = 0.50;

    ext::shared_ptr<StochasticProcessArray> theProcess (
    		new StochasticProcessArray(processes, correlationMatrix));

    //create Monte Carlo pricing engine
    Size mcSeed = 42;
    ext::shared_ptr<PricingEngine> mcBasketEngine;
    mcBasketEngine =
    		MakeMCEuropeanBasketEngine<PseudoRandom> (theProcess)
			.withSteps(1)
			.withSamples(10000)
			.withSeed(mcSeed);

    //set pricing engine for the basket option and calculate price
    basketOption.setPricingEngine(mcBasketEngine);
    Real price = basketOption.NPV();

	std::cout << std::setw(widths[0]) << std::left << "Monte Carlo"
	                  << std::fixed
	                  << std::setw(widths[1]) << std::left << price
	                  << std::endl;

return 0;
}







