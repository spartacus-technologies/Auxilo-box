#include "ClientObserver.hpp"
#include "ClientWrapper.hpp"

ClientObserver::ClientObserver(): wrapper_(nullptr)
{
}

ClientObserver::~ClientObserver()
{
    wrapper_ = nullptr;
}

void ClientObserver::addSubject(ClientWrapper *ptr)
{
    wrapper_ = ptr;
    wrapper_->addObserver(this);
}
