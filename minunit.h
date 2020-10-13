#ifndef MINUNIT
#define MINUNIT

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define mu_assert(message, test) do { if (!(test)) return "TEST FAIL (line " TOSTRING(__LINE__) "): " message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                 if (message) return message; } while (0)
extern int tests_run;

#endif
