
#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP
#include <stdexcept>
using std::runtime_error;

class SignalException : public runtime_error
{
public:
   SignalException(const std::string& _message)
      : std::runtime_error(_message)
   {}
};

class SignalHandler
{
protected:
    static bool mbGotExitSignal;

public:
    SignalHandler();
    ~SignalHandler();

    static bool gotExitSignal();
    static void setExitSignal(bool _bExitSignal);

    void        setupSignalHandlers();
    static void exitSignalHandler(int _ignored);

};
#endif //SIGNAL_HANDLER_HPP
