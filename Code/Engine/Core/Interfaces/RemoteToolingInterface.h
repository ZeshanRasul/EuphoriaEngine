#pragma once

class plRemoteInterface;

/// \brief Interface to give access to the FileServe client for additional tooling needs.
///
/// For now, this interface just gives access to the plRemoteInterface that is used to communicate with the FileServe server.
/// This allows for maximum flexibility sending and receiving custom messages.
class plRemoteToolingInterface
{
public:
  virtual plRemoteInterface* GetRemoteInterface() = 0;
};