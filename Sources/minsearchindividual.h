#ifndef MINSEARCHINDIVIDUAL_H
#define MINSEARCHINDIVIDUAL_H

#include "log.h"
#include "individual.h"
#include "dnatypes.h"

namespace Genetic
{
	template <class F>
	class MinSearchIndividual : public Genetic::Individual <LinearRealDna>
	{
	public:
		MinSearchIndividual(bool generateDna = false);
		static void recombine(Genetic::Individual <LinearRealDna>* parentIndividual1,
		                      Genetic::Individual <LinearRealDna>* parentIndividual2,
		                      Genetic::Individual <LinearRealDna>* childIndividual1,
		                      Genetic::Individual <LinearRealDna>* childIndividual2);
		void mutate();
		void test();
	};
}

template <class F>
Genetic::MinSearchIndividual <F>::MinSearchIndividual(bool generateDna)
{
	if(generateDna)
	{
		dna.generate(2);
	} else {
		dna.resize(2);
	}
}

template <class F>
void Genetic::MinSearchIndividual <F>::mutate()
{
	dna.mutate(5.0);
}

template <class F>
void Genetic::MinSearchIndividual <F>::test()
{
	score = (F::getResult(dna));
}

template <typename F>
void Genetic::MinSearchIndividual <F>::recombine(Genetic::Individual <LinearRealDna>* parentIndividual1,
                                                 Genetic::Individual <LinearRealDna>* parentIndividual2,
                                                 Genetic::Individual <LinearRealDna>* childIndividual1,
                                                 Genetic::Individual <LinearRealDna>* childIndividual2)
{
	Individual <LinearRealDna>::recombine(parentIndividual1,
	                                      parentIndividual2,
	                                      childIndividual1,
	                                      childIndividual2,
	                                      CROSSOVER, 2);
}


#endif // MINSEARCHINDIVIDUAL_H