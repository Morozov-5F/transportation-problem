#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>

#define PRINT_FORMAT_ERROR            "\x1B[31m"
#define PRINT_FORMAT_HIGHLIGHT        "\x1B[35m"
#define PRINT_FORMAT_HIGHLIGHT_2      "\x1B[33m"
#define PRINT_CLEAR_FORMAT            "\x1B[0m"

#define FILE_NAME                     "input.txt"

static void dump_matrix(gsl_matrix * M)
{
   size_t m = M->size1, n = M->size2;
   int i, j;
   for (i = 0; i < m; ++i)
   {
      for (j = 0; j < n; ++j)
      {
         double elem = gsl_matrix_get(M, i, j);
         printf("% 03.2f\t", elem);
      }
      printf("\n");
   }
   printf("\n");
}

void north_west_method(gsl_matrix * weight_matrix,
                       gsl_matrix * result);

void lowest_cost_method(gsl_matrix * weight_matrix,
                         gsl_matrix * result);

void fogel_method(gsl_matrix * weight_matrix,
                  gsl_matrix * result);

int main(int argc, char const * argv [])
{
   unsigned m, n;
   gsl_matrix * matrix;
   gsl_matrix * initial_assumption;

   FILE * input_file = fopen(FILE_NAME, "r");

   if (!input_file)
   {
      fprintf(stderr, PRINT_FORMAT_ERROR "Error has occured while openning file %s.\n" PRINT_CLEAR_FORMAT, FILE_NAME);
      goto done;
   }

   fscanf(input_file, "%u %u", &m, &n);
   initial_assumption = gsl_matrix_alloc(m, n);
   matrix = gsl_matrix_alloc(m + 1, n + 1);
   gsl_matrix_fscanf(input_file, matrix);

   fclose(input_file);
   printf("Weights:\n");
   dump_matrix(matrix);
   printf("Initial assumption (north-west):\n");
   gsl_matrix_set_all(initial_assumption, NAN);
   north_west_method(matrix, initial_assumption);
   dump_matrix(initial_assumption);
   gsl_matrix_set_all(initial_assumption, NAN);
   printf("Initial assumption (lowest cost):\n");
   lowest_cost_method(matrix, initial_assumption);
   dump_matrix(initial_assumption);
done:
   gsl_matrix_free(matrix);
   gsl_matrix_free(initial_assumption);

   return GSL_SUCCESS;
}

void north_west_method(gsl_matrix * weight_matrix,
                       gsl_matrix * result)
{
   gsl_vector * demand, * offer;
   unsigned m = weight_matrix->size1;
   unsigned n = weight_matrix->size2;
   unsigned i = 0, j = 0;

   offer  = gsl_vector_alloc(m);
   demand = gsl_vector_alloc(n);

   gsl_matrix_get_row(demand, weight_matrix, m - 1);
   gsl_matrix_get_col(offer,  weight_matrix, n - 1);

   gsl_vector_set(offer, m - 1, 0);
   gsl_vector_set(demand, n - 1, 0);

   while (!gsl_vector_isnull(offer) || !gsl_vector_isnull(demand))
   {
      double min = GSL_MIN(gsl_vector_get(offer, i), gsl_vector_get(demand, j));
      double * offer_elem = gsl_vector_ptr(offer, i);
      double * demand_elem = gsl_vector_ptr(demand, j);

      gsl_matrix_set(result, i, j, min);

      *offer_elem -= min;
      *demand_elem -= min;

      i += (*offer_elem == 0);
      j += (*demand_elem == 0);
   }

   gsl_vector_free(demand);
   gsl_vector_free(offer);
}

void lowest_cost_method(gsl_matrix * weight_matrix,
                         gsl_matrix * result)
{
   gsl_matrix * buffer;
   gsl_vector * demand, * offer;
   gsl_vector_view line;
   gsl_matrix_view submatrix;
   double max = 0;
   unsigned m = weight_matrix->size1;
   unsigned n = weight_matrix->size2;
   size_t i = 0, j = 0;
   int already_swapped = 0;

   max = gsl_matrix_max(weight_matrix);

   buffer = gsl_matrix_alloc(m - 1, n - 1);
   offer  = gsl_vector_alloc(m);
   demand = gsl_vector_alloc(n);

   gsl_matrix_get_row(demand, weight_matrix, m - 1);
   gsl_matrix_get_col(offer,  weight_matrix, n - 1);

   gsl_vector_set(offer, m - 1, 0);
   gsl_vector_set(demand, n - 1, 0);

   submatrix = gsl_matrix_submatrix(weight_matrix, 0, 0,  m - 1, n - 1);
   gsl_matrix_memcpy(buffer, &submatrix.matrix);

   while (!gsl_vector_isnull(offer) || !gsl_vector_isnull(demand))
   {
      double min = 0;
      double * offer_elem, * demand_elem;

      gsl_matrix_min_index(buffer, &i, &j);

      min = GSL_MIN(gsl_vector_get(offer, i), gsl_vector_get(demand, j));
      offer_elem = gsl_vector_ptr(offer, i);
      demand_elem = gsl_vector_ptr(demand, j);

      gsl_matrix_set(result, i, j, min);

      *offer_elem -= min;
      *demand_elem -= min;

      if (*offer_elem == 0)
      {
         line = gsl_matrix_row(buffer, i);
         gsl_vector_set_all(&line.vector, max + 1);
      }
      else if (*demand_elem == 0)
      {
         line = gsl_matrix_column(buffer, j);
         gsl_vector_set_all(&line.vector, max + 1);
      }
   }

   gsl_vector_free(demand);
   gsl_vector_free(offer);
   gsl_matrix_free(buffer);
}
