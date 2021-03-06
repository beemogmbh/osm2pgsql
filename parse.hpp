#ifndef PARSE_H
#define PARSE_H

#include "osmtypes.hpp"

#include <time.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

typedef enum { FILETYPE_NONE, FILETYPE_OSM, FILETYPE_OSMCHANGE, FILETYPE_PLANETDIFF } filetypes_t;
typedef enum { ACTION_NONE, ACTION_CREATE, ACTION_MODIFY, ACTION_DELETE } actions_t;

class parse_t;
class osmdata_t;
struct reprojection;

class parse_delegate_t
{
public:
    parse_delegate_t(const int extra_attributes, const boost::optional<std::string> &bbox, boost::shared_ptr<reprojection> projection);
	~parse_delegate_t();

	int streamFile(const char* input_reader, const char* filename, const int sanitize, osmdata_t *osmdata);
	void printSummary() const;
	boost::shared_ptr<reprojection> getProjection() const;

private:
	parse_delegate_t();
        void parse_bbox(const std::string &bbox);
	parse_t* get_input_reader(const char* input_reader, const char* filename);

	const int m_extra_attributes;
	boost::shared_ptr<reprojection> m_proj;
	osmid_t m_count_node, m_max_node;
	osmid_t m_count_way,  m_max_way;
	osmid_t m_count_rel,  m_max_rel;
	time_t  m_start_node, m_start_way, m_start_rel;

	bool m_bbox;
	double m_minlon, m_minlat, m_maxlon, m_maxlat;
};

class parse_t
{
	friend class parse_delegate_t;

public:
	parse_t(const int extra_attributes_, const bool bbox_, const boost::shared_ptr<reprojection>& projection_,
			const double minlon, const double minlat, const double maxlon, const double maxlat);
	virtual ~parse_t();

	virtual int streamFile(const char *filename, const int sanitize, osmdata_t *osmdata) = 0;

protected:
    parse_t();

    void print_status();

    int node_wanted(double lat, double lon)
    {
        if (!bbox)
            return 1;

        if (lat < minlat || lat > maxlat)
            return 0;
        if (lon < minlon || lon > maxlon)
            return 0;
        return 1;
    }

	osmid_t count_node,    max_node;
	osmid_t count_way,     max_way;
	osmid_t count_rel,     max_rel;
	time_t  start_node, start_way, start_rel;

    /* Since {node,way} elements are not nested we can guarantee the
    values in an end tag must match those of the corresponding
    start tag and can therefore be cached.
    */
    double node_lon, node_lat;
    taglist_t tags;
    idlist_t nds;
    memberlist_t members;
    osmid_t osm_id;
    filetypes_t filetype;
    actions_t action;
    int parallel_indexing;

	const int extra_attributes;
	mutable bool bbox;
	mutable double minlon, minlat, maxlon, maxlat;
	const boost::shared_ptr<reprojection> proj;
};

#endif
