
#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "genetic.h"

namespace Genetic {


/**
 * class Individual
 *
 */

	template <typename D>
	class Individual
	{
	public:

		/**
		 * Constructor
		 * @param generateDna shuold the constructor generate dna
		 * @param dnaGenerateParameter parametr for dna generator (for example: size of linear dna)
		 */
        Individual(bool generateDna = false, double dnaGenerateParameter = 6.0);

		/// Empty Destructor.
		virtual ~Individual();

		virtual void test() = 0;

		/// Recombine method.
		static void recombine(Genetic::Individual <D>* parent_individual1,
		                      Genetic::Individual <D>* parent_individual2,
		                      Genetic::Individual <D>* child_individual1,
		                      Genetic::Individual <D>* child_individual2,
		                      Genetic::RecombinationType rec_type,
		                      double recombine_param = 1.0);

		/// Mutate method.
		void mutate(double probability, int attempts, double parameter);

		/// Generate method.
		virtual void generate();

		/// Update parameters from dna.
		virtual void updateParameters();

		/**
		 * Set the value of score
		 * @param new_var the new value of score
		 */
		void setScore(double value);

		/**
		 * Get the value of score
		 * @return the value of score
		 */
		double getScore();

		/**
		 * Set the value of dna
		 * @param new_var the new value of dna
		 */
		void setDna(D& value);

		/**
		 * Get the value of dna
		 * @return the value of dna
		 */
		D* getDna();

	protected:

		// Static Protected attributes
		//

		// Protected attributes
		//

		double score;	///< Score value
		D dna;
	};
};

template <typename D>
Genetic::Individual <D>::Individual(bool generateDna, double dnaGenerateParameter)
{
    if(generateDna)
	{
        dna.generate(dnaGenerateParameter);
	}
}

template <typename D>
Genetic::Individual <D>::~Individual()
{
}

template <typename D>
void Genetic::Individual <D>::recombine(Genetic::Individual <D>* parent_individual1,
                                        Genetic::Individual <D>* parent_individual2,
                                        Genetic::Individual <D>* child_individual1,
                                        Genetic::Individual <D>* child_individual2,
                                        Genetic::RecombinationType rec_type,
                                        double recombine_param)
{
    assert(parent_individual1 -> dna.size() == parent_individual2 -> dna.size());
    child_individual1 -> dna.resize(parent_individual1 -> dna.size());
    child_individual2 -> dna.resize(parent_individual1 -> dna.size());

	switch(rec_type)
	{
	case DISCRETE:
	{
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			if(rand() & 1)
			{
				child_individual1 -> dna[i] = parent_individual1 -> dna[i];
			} else {
				child_individual1 -> dna[i] = parent_individual2 -> dna[i];
			}
			if(rand() & 1)
			{
				child_individual2 -> dna[i] = parent_individual1 -> dna[i];
			} else {
				child_individual2 -> dna[i] = parent_individual2 -> dna[i];
			}
		}
		break;
	}
	case INTERMEDIATE:
	{
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			double alpha1 = static_cast<double>(rand() % 1000) / 1000.0 * (1.0 + 2.0 * recombine_param) - recombine_param;
			double alpha2 = static_cast<double>(rand() % 1000) / 1000.0 * (1.0 + 2.0 * recombine_param) - recombine_param;
			double v[2];
			v[0] = parent_individual1 -> dna[i] + alpha1 * (parent_individual2 -> dna[i] - parent_individual1 -> dna[i]);
			v[1] = parent_individual1 -> dna[i] + alpha2 * (parent_individual2 -> dna[i] - parent_individual1 -> dna[i]);
			child_individual1 -> dna[i] = v[0];
			child_individual2 -> dna[i] = v[1];
		}
		break;
	}
	case LINE:
	{
		double alpha1 = static_cast<double>(rand() % 1000) / 1000.0 * (1.0 + 2.0 * recombine_param) - recombine_param;
		double alpha2 = static_cast<double>(rand() % 1000) / 1000.0 * (1.0 + 2.0 * recombine_param) - recombine_param;
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			double v[2];
			v[0] = parent_individual1 -> dna[i] + alpha1 * (parent_individual2 -> dna[i] - parent_individual1 -> dna[i]);
			v[1] = parent_individual1 -> dna[i] + alpha2 * (parent_individual2 -> dna[i] - parent_individual1 -> dna[i]);
			child_individual1 -> dna[i] = v[0];
			child_individual2 -> dna[i] = v[1];
		}
		break;
	}
	case CROSSOVER:
	{
	    int crossover_points_num = recombine_param;
		assert(parent_individual1 -> dna.size() > 1);
		assert(parent_individual1 -> dna.size() >= crossover_points_num);
		assert(crossover_points_num > 0);

		// Setting dividing points
		std::vector <int> free_points(parent_individual1 -> dna.size());
		std::vector <int> crossover_points(crossover_points_num);
		for(int i = 0; i < free_points.size(); ++i)
		{
			free_points[i] = i;
		}
		for(int i = 0; i < crossover_points_num; ++i)
		{
			int choosed_point_id = rand() % free_points.size();
			int choosed_point = free_points[choosed_point_id];
			free_points.erase(free_points.begin() + choosed_point_id);
			crossover_points[i] = choosed_point;
		}

		std::sort(crossover_points.begin(), crossover_points.end());
		if(crossover_points[crossover_points.size() - 1]
		   != parent_individual1 -> dna.size() - 1)
		{
			crossover_points.push_back(parent_individual1 -> dna.size() - 1);
		}

		int last_crossover_point = 0;
		int next_crossover_point;
		bool current_inverse = false;

		for(int i = 0; i < crossover_points.size(); ++i)
		{
			int crossover_point = crossover_points[i];
			next_crossover_point = (i < crossover_points_num - 1) ?
				crossover_points[i + 1] :
				parent_individual1 -> dna.size();


			if(current_inverse)
			{
				for(int j = last_crossover_point; j <= crossover_point; ++j)
				{
					child_individual1 -> dna[j] = parent_individual2 -> dna[j];
					child_individual2 -> dna[j] = parent_individual1 -> dna[j];
				}
			} else {
				for(int j = last_crossover_point; j <= crossover_point; ++j)
				{
					child_individual1 -> dna[j] = parent_individual1 -> dna[j];
					child_individual2 -> dna[j] = parent_individual2 -> dna[j];
				}
			}

			last_crossover_point = crossover_point;
			current_inverse = !current_inverse;
		}
		break;
	}
	case UNIFORM_CROSSOVER:
	{
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			if(rand() & 1)
			{
				child_individual1 -> dna[i] = parent_individual1 -> dna[i];
				child_individual2 -> dna[i] = parent_individual2 -> dna[i];
			} else {
				child_individual1 -> dna[i] = parent_individual2 -> dna[i];
				child_individual2 -> dna[i] = parent_individual1 -> dna[i];
			}
		}
		break;
	}
	case TRIADIC_CROSSOVER:

		break;
	case SHUFFLER_CROSSOVER:
	{
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			if(rand() & 1)
			{
				std::swap(parent_individual1 -> dna[i], parent_individual2 -> dna[i]);
			}
		}
		recombine(parent_individual1, parent_individual2,
		          child_individual1, child_individual2,
		          CROSSOVER, recombine_param);
		for(int i = 0; i < parent_individual1 -> dna.size(); ++i)
		{
			if(rand() & 1)
			{
				std::swap(child_individual1 -> dna[i], child_individual2 -> dna[i]);
			}
		}
		break;
	}
	case CROSSOVER_WITH_REDUCED_SURROGATE:

		break;
	}
}

template <typename D>
void Genetic::Individual <D>::mutate(double probability, int attempts, double parameter)
{
	for(int attemptNo = 0; attemptNo < attempts; ++attemptNo)
	{
	    double rndVal = static_cast<double>(rand() % 10000) / 10000.0;
		if(rndVal <= probability)
		{
		    dna.mutate(parameter);
		}
	}
}

template <typename D>
void Genetic::Individual <D>::generate()
{
}

template <typename D>
void Genetic::Individual <D>::updateParameters()
{
}

template <typename D>
void Genetic::Individual <D>::setScore(double value)
{
	score = value;
}

template <typename D>
double Genetic::Individual <D>::getScore()
{
	return score;
}

template <typename D>
void Genetic::Individual <D>::setDna(D& value)
{
	dna = value;
}

template <typename D>
D* Genetic::Individual <D>::getDna()
{
	return &dna;
}

#endif // INDIVIDUAL_H