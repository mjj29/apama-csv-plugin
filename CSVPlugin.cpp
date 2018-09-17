#include<cinttypes>
#include<epl_plugin.hpp>
#include<sag_connectivity_plugins.hpp>
#include<CSVParser.hpp>
#include<string>

using com::softwareag::connectivity::Logger;
using com::softwareag::connectivity::AbstractCodec;
using com::softwareag::connectivity::PluginConstructorParameters;
using com::softwareag::connectivity::AbstractSimpleCodec;
using com::softwareag::connectivity::Message;
using com::softwareag::connectivity::list_t;
using com::softwareag::connectivity::map_t;
using com::softwareag::connectivity::data_t;
using com::softwareag::connectivity::get;
using com::softwareag::connectivity::convert_to;
using com::apama::epl::EPLPlugin;

namespace com::apamax {

class CSVCodec: public AbstractSimpleCodec, public EPLPlugin<CSVCodec>
{
public:
	explicit CSVCodec(const CodecConstructorParameters &param)
		: AbstractSimpleCodec(param), base_plugin_t("CSVPlugin")
	{}
	explicit CSVCodec()
		: AbstractSimpleCodec(CodecConstructorParameters("CSVCodec", "", map_t{}, nullptr, nullptr)),
		  base_plugin_t("CSVPlugin")
	{}
	static void initialize(base_plugin_t::method_data_t &md)
	{
		md.registerMethod<decltype(&CSVCodec::decode), &CSVCodec::decode>("decode", "action<string, string> returns sequence<sequence<string> >");
		md.registerMethod<decltype(&CSVCodec::encode), &CSVCodec::encode>("encode", "action<sequence<sequence<string> >, string> returns string");
	}
	list_t decode(const std::string &str, const char *delimiter)
	{
		csv::Parser csv{str, csv::ePURE, delimiter[0]};
		list_t l{};
		for (size_t i = 0; i < csv.rowCount(); ++i) {
			list_t r{};
			for (size_t j = 0; j < csv[i].size(); ++j) {
				r.push_back(csv[i][j]);
			}
			l.push_back(data_t{std::move(r)});
		}
		return l;
	}
	std::string encode(const list_t &l, const char *delimiter)
	{
		std::ostringstream oss;
		for (auto it = l.begin(); it != l.end(); ++it) {
			const list_t &r = get<list_t>(*it);
			for (auto jt = r.begin(); jt != r.end(); ++jt) {
				if (jt != r.begin()) oss << delimiter[0];
				std::string val = convert_to<std::string>(*jt);
				if (std::string::npos != val.find(delimiter)) {
					oss << '"' << val << '"';
				} else {
					oss << val;
				}
			}
			oss << std::endl;
		}
		return oss.str();
	}
	virtual bool transformMessageTowardsTransport(Message &m) override
	{
		m.setPayload(encode(get<list_t>(m.getPayload()), &delimiter));
		return true;
	}
	virtual bool transformMessageTowardsHost(Message &m) override
	{
		m.setPayload(decode(get<const char*>(m.getPayload()), &delimiter));
		return true;
	}
private:
	char delimiter = ',';
};

SAG_DECLARE_CONNECTIVITY_CODEC_CLASS(CSVCodec)
APAMA_DECLARE_EPL_PLUGIN(CSVCodec)

} // com::apamax

