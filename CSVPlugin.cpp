#include<epl_plugin.hpp>
#include<sag_connectivity_plugins.hpp>
#include<rapidcsv.h>
#include<string>

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
		list_t l{};
		std::istringstream iss{str};
		rapidcsv::Document doc(iss, str.size(), rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(delimiter[0]));
		for (size_t i = 0; i < doc.GetRowCount(); ++i) {
			auto row = doc.GetRow<std::string>(i);
			list_t r{};
			for (auto it = row.begin(); it != row.end(); ++it) {
				if (it->at(0) == '"' && it->at(it->length()-1) == '"') {
					r.push_back(it->substr(1, it->length()-2));
				} else {
					r.push_back(*it);
				}
			}
			l.push_back(std::move(r));
		}
		return l;
	}
	std::string encode(const list_t &l, const char *delimiter)
	{
		rapidcsv::Document doc("", rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(delimiter[0]));
		size_t rows = 0;
		for (auto it = l.begin(); it != l.end(); ++it) {
			const auto &r = get<list_t>(*it);
			std::vector<std::string> row{};
			for (auto jt = r.begin(); jt != r.end(); ++jt) {
				row.push_back(get<const char*>(*jt));
			}
			doc.SetRow(rows++, row);
		}
		std::ostringstream oss;
		doc.Save(oss);
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

