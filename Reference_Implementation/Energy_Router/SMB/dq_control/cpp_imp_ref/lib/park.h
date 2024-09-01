#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <map>
#include <vector>
#include <array>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <assert.h>     /* assert */
#include <fstream>

# define PI 3.141592653589 

void park_transform_1phase(float alpha, float beta, float &d, float &q, float wt)
{
   d = cos(wt) * alpha - sin(wt)*beta;
   q = sin(wt) * alpha + cos(wt)*beta;
}