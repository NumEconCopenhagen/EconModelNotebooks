// functions for writing to logs.
//
// call: e.g logs::write(log_file,print_level,text);  
//  if print_level < PRINT_LEVEL the print statement is ignored
//  else text is printed to log_solve.txt
//  if print_level == 0 the file is wiped beforehand

#define PRINT_LEVEL LONG_MAX
namespace logs {

void create(const char *filename)
{

  #if PRINT_LEVEL >= 0

    FILE* log_file;
    log_file = fopen(filename,"w"); // write -> empty file
    fclose(log_file);

  #endif

}

void write(const char *filename, int print_level, const char *txt, ... )
{

  #if PRINT_LEVEL >= 0

    // a. determine behavior
    FILE* log_file;
    if(print_level <= PRINT_LEVEL){
        log_file = fopen(filename,"a"); // append
    } else { // nothing
        return;
    }
    if(log_file == nullptr) return;

    // b. print
    va_list args;
    va_start (args, txt);
    vfprintf (log_file, txt, args);

    // c. close down
    fclose(log_file);
    va_end (args);

  #endif

}

} // namespace