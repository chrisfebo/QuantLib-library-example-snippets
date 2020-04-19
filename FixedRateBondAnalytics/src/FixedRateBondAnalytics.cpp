/* FIXED RATE BOND ANALYTICS */

/* This example sets up a fixed rate bond and outputs:
 * yield to maturity, accrued interest at the time of evaluation,
 *  and full coupon schedule with amounts. */

#include <ql/qldefines.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/instruments/bonds/fixedratebond.hpp>
#include <ql/cashflows/cashflows.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/thirty360.hpp>

#include <iostream>

using namespace QuantLib;

int main (int, char *[]) {

	//define date, calendar, and payment convention
	Calendar calendar = UnitedStates();
	DayCounter dayCounter = Thirty360();

	Date todaysDate (14 ,March ,2019);
	Settings::instance().evaluationDate() = todaysDate;

	Natural settlementDays = 0;

    BusinessDayConvention paymentConvention = Unadjusted;

    //define the bond specific data
	Date issueDate (31, August, 2017);
	Date maturityDate (31, August, 2022);

	Period period = Period(6, Months);

	Real faceValue = 100;
	Real redemptionValue = 100;

	Real coupon = 0.03125;
	std::vector<Rate> fixedRateCoupons(1,coupon);

	Real cleanPrice = 97.989976;

	Compounding compounding = Simple;
	Frequency frequency = Semiannual;

	//define the bond coupons schedule
	Schedule fixedRateSchedule(
			issueDate,
			maturityDate,
			period,
			calendar,
			paymentConvention,
			paymentConvention,
			DateGeneration::Backward,
			false
			);

	//define the bond
	FixedRateBond fixedRateBond(
			settlementDays,
			faceValue,
			fixedRateSchedule,
			fixedRateCoupons,
			dayCounter,
			paymentConvention,
			redemptionValue,
			issueDate);

	//calculate yield to maturity
	Real accuracy = 0.0000001;

	Real accruedAmount = fixedRateBond.accruedAmount(todaysDate);

	Leg bondCashFlows = fixedRateBond.cashflows();

	Rate ytm1 = fixedRateBond.yield(cleanPrice, dayCounter, compounding, frequency, todaysDate, accuracy);

	//output ytm and accrued interest
	std::cout << "yield to maturity: " << ytm1 << std::endl;
	std::cout << "accrued interest: " << accruedAmount << std::endl;

	//outpot coupon cashflows
	std::cout << "Cashflows" << std::endl;
	for (Size i=0; i<bondCashFlows.size()-1; i++) {
	                Date date =  bondCashFlows[i]->date();
	                Real value = bondCashFlows[i]->amount();
	                std::cout << " date: " << date << " value: " << value << std::endl;
	        }


}
