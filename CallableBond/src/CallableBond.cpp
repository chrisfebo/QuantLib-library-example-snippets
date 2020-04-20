/* CALLABLE BOND PRICING */

/* This example sets up a Callable bond with quarterly callability and coupon schedule.
 * The bond is priced using a Tree based pricing engine. The interest rate model used is
 * 1-D Hull-White. */

#include <ql/qldefines.hpp>
#include <ql/experimental/callablebonds/callablebond.hpp>
#include <ql/experimental/callablebonds/treecallablebondengine.hpp>
#include <ql/models/shortrate/onefactormodels/hullwhite.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/utilities/dataformatters.hpp>
#include <ql/time/schedule.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace QuantLib;

int main (int, char* []) {

	//set calendar framework
	Date todaysDate (25,February,2019);
	Date issueDate (25,February,2019);
	Date maturityDate = issueDate + Period(10,Years);
	Natural settlementDays = 2;

	DayCounter dayCounter = ActualActual(ActualActual::Bond);
	Calendar calendar = TARGET();
    Settings::instance().evaluationDate() = todaysDate;

	//set bond parameters and market data
    Real riskFreeRate = 0.0275;
    Real faceValue = 100;
    Real redemption = 100;
    Real coupon = 0.05;
    vector<Rate> coupons(1,coupon);
    Real callPrice = 102;
    Frequency frequency = Quarterly;

    // print information
    cout << "Today = " << todaysDate << endl;
    cout << "Issuance = " << issueDate << endl;
    cout << "Maturity = " << maturityDate << endl;
    cout << "Risk-free rate = " << riskFreeRate << endl;
    cout << "Face value = " << faceValue << endl;
    cout << "Coupon = " << coupon << endl;
    cout << "Call price = " << callPrice << endl;
    cout << "Frequency = " << frequency << endl;
    cout << endl;

    //create yield curve
    Handle<YieldTermStructure> termStructure(ext::shared_ptr<YieldTermStructure>(
    		new FlatForward(issueDate, riskFreeRate, dayCounter)));

    //create callability schedule
    CallabilitySchedule callSchedule;
    Size numberOfCallDates = 38;
    Date callDate = issueDate + settlementDays + Period(3, Months);

    for (Size i = 0; i < numberOfCallDates; i++) {
    	Callability::Price price(callPrice, Callability::Price::Clean);
    	callSchedule.push_back(ext::make_shared<Callability> (price, Callability::Call, callDate));
    	callDate = callDate + Period(3,Months);
    }

    //set conventions for accrued interest and payment
    BusinessDayConvention accrualConvention = Following;
    BusinessDayConvention paymentConvention = Following;

    //create instrument
    Schedule schedule(issueDate, maturityDate, Period(frequency), calendar,
    		accrualConvention, accrualConvention, DateGeneration::Backward, false);

    CallableFixedRateBond callableBond (settlementDays, faceValue, schedule,
    		coupons, dayCounter, paymentConvention, redemption, issueDate, callSchedule);

    //model parameters for Hull-White model
    Real reversionParameter = 0.03;
    Real sigma = 0.1;
    Integer timeSteps = 100;

    //create model
    ext::shared_ptr<ShortRateModel> hullWhite(
    		new HullWhite(termStructure, reversionParameter, sigma));

    //create pricing engine with Tree method
    ext::shared_ptr<PricingEngine> treeEngine(
    		new TreeCallableFixedRateBondEngine(hullWhite, timeSteps));

    callableBond.setPricingEngine(treeEngine);

    //price
    cout << "NPV: " << callableBond.NPV() << endl;

	return 0;
}



