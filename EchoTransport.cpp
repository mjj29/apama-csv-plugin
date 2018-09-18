#include<sag_connectivity_plugins.hpp>
#include<rapidcsv.h>
#include<string>

using com::softwareag::connectivity::AbstractSimpleTransport;
using com::softwareag::connectivity::Message;

namespace com::apamax {

class EchoTransport: public AbstractSimpleTransport
{
public:
	explicit EchoTransport(const TransportConstructorParameters &param)
		: AbstractSimpleTransport(param)
	{}
	virtual void deliverMessageTowardsTransport(Message &m) override
	{
		hostSide->sendBatchTowardsHost(&m, &m+1);
	}
};

SAG_DECLARE_CONNECTIVITY_TRANSPORT_CLASS(EchoTransport)

} // com::apamax

