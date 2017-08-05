#include <piControlIf.hpp>
#include <piControl.h>
#include <stdio.h>

class IOHandler {
    private:
        static piControl piCtrl;

    public:
        static bool GetIO(const char* name);
        static int SetIO(const char* name, bool on);
};  