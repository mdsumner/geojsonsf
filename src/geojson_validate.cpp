
#include "rapidjson/document.h"
#include <string.h>

#include <Rcpp.h>
using namespace Rcpp;
using namespace rapidjson;

void geojson_object_error(std::string key) {
  std::string err = "Invalid " + key + " object";
  Rcpp::stop(err);
}

void geojson_object_error(std::string key, int sfg_number) {
  std::string err = "No '" + key + "' member at object index " + std::to_string(sfg_number) + " - invalid GeoJSON";
  Rcpp::stop(err);
}

void safe_parse(Document& d, const char* geojson) {
  d.Parse(geojson);
  if( d.Parse(geojson).HasParseError() ) {
    Rcpp::stop("Invalid JSON");
  }
}

void validate_array(const Value& v) {
  if ( v.IsArray() == false) {
    geojson_object_error("array");
  }
}

void validate_array(const Value& v, int& sfg_objects) {
  if ( v.IsArray() == false) {
    geojson_object_error("array", sfg_objects);
  }
}


void validate_type(const Value& v, int& sfg_objects) {
  if (v.HasMember("type") == false ) {
    geojson_object_error("type", sfg_objects);
  }
  if (v["type"].IsNull()) {
  	geojson_object_error("type", sfg_objects);
  }
}

void validate_features(const Value& v, int& sfg_objects) {
  if (v.HasMember("features") == false) {
    geojson_object_error("features", sfg_objects);
  }
}

void validate_feature(const Value& v, int& sfg_objects) {
  if (v.HasMember("feature") == false) {
    geojson_object_error("feature", sfg_objects);
  }
}

void validate_properties(const Value& v, int& sfg_objects) {
  if (v.HasMember("properties") == false) {
    geojson_object_error("properties", sfg_objects);
  }
}

void validate_geometry(const Value& v, int& sfg_objects) {
  if (v.HasMember("geometry") == false) {
    geojson_object_error("geometry", sfg_objects);
  }
}

void validate_geometries(const Value& v, int& sfg_objects) {
  if (v.HasMember("geometries") == false) {
    geojson_object_error("geometries", sfg_objects);
  }
}

void validate_coordinates(const Value& v, int& sfg_objects) {
  if (v.HasMember("coordinates") == false) {
    geojson_object_error("coordinates", sfg_objects);
  }
}

void validate_points(const Value& v) {
	if ( v.Size() < 2 || v.Size() > 4 ) {
		geojson_object_error("lon/lat");
	}
}

void validate_point(const Value& v) {
	// TODO(move to header):
	// Rcpp::Rcout << "v.size: " << v.Size() << std::endl;
	// Rcpp::Rcout << "v.type: " << v.GetType() << std::endl;
	// if ( v.Size() < 2 ) {
	// 	Rcpp::stop("mis-specified geometry");
	// }

	static const char* ARRAY_TYPES[] =
		{ "Null", "false", "True", "Object", "Array", "String", "Number" };
	if( strncmp(ARRAY_TYPES[v.GetType()], "Num", 3) != 0) {
		geojson_object_error("lon/lat");
	}
}

