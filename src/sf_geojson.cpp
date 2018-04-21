#include "geojsonsf.h"
#include "geojson_wkt.h"
#include "sf_geojson.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <Rcpp.h>
using namespace Rcpp;

template <int RTYPE>
Rcpp::CharacterVector sfClass(Vector<RTYPE> v) {
	return v.attr("class");
}

Rcpp::CharacterVector getSfClass(SEXP sf) {
	switch( TYPEOF(sf) ) {
	case REALSXP:
		return sfClass<REALSXP>(sf);
	case VECSXP:
		return sfClass<VECSXP>(sf);
	case INTSXP:
		return sfClass<INTSXP>(sf);
	default: Rcpp::stop("unknown sf type");
	}
	return "";
}


void get_column_type(Rcpp::List& sf,
                     Rcpp::StringVector& property_names,
                     Rcpp::StringVector& column_types) {

	for (int i = 0; i < property_names.size(); i++) {

		Rcpp::String col = property_names[i];
		SEXP vec = sf[col];

		switch(TYPEOF(vec)) {
		case REALSXP:
			column_types[i] = "Number";
			break;
		case INTSXP:
			column_types[i] = "Number";
			break;
		case LGLSXP:
			column_types[i] = "Logical";
			break;
		default: {
				column_types[i] = "String";
				break;
		}
		}
	}
}

void begin_geojson_geometry(Rcpp::String& geojson, std::string& geom_type) {
    Rcpp::List sfc;
	  begin_geojson_geometry(geojson, sfc, geom_type);
}

void begin_geojson_geometry(Rcpp::String& geojson, Rcpp::List& sfc, std::string& geom_type) {

	geojson += "{\"type\":";
	if (geom_type == "POINT") {
 		geojson +=  "\"Point\",\"coordinates\":";
	} else if (geom_type == "MULTIPOINT") {
		geojson += "\"MultiPoint\",\"coordinates\":[";
	} else if (geom_type == "LINESTRING") {
		geojson += "\"LineString\",\"coordinates\":[";
	} else if (geom_type == "MULTILINESTRING") {
		geojson += "\"MultiLineString\",\"coordinates\":[[";
	} else if (geom_type == "POLYGON") {
    geojson += "\"Polygon\",\"coordinates\":[[";
	} else if (geom_type == "MULTIPOLYGON") {
		geojson += "\"MultiPolygon\",\"coordinates\":[[[";
	} else if (geom_type == "GEOMETRYCOLLECTION") {
		geojson += "\"GeometryCollection\",\"geometries\":[";
	}
}

void end_geojson_geometry(Rcpp::String& geojson, std::string& geom_type) {
	if (geom_type == "POINT") {
		geojson += "}";
	} else if (geom_type == "MULTIPOINT") {
		geojson += "]}";
	} else if (geom_type == "LINESTRING") {
		geojson += "]}";
	} else if (geom_type == "MULTILINESTRING") {
		geojson += "]]}";
	} else if (geom_type == "POLYGON") {
		geojson += "]]}";
	} else if (geom_type == "MULTIPOLYGON") {
		geojson += "]]]}";
	} else if (geom_type == "GEOMETRYCOLLECTION") {
		geojson += "]}";
	}
}

void coord_separator(Rcpp::String& geojson, int i, int n) {
	if (i < (n - 1) ) {
		geojson += ",";
	}
}

void object_separator(Rcpp::String& geojson, int i, int n) {
	geojson += ",";
}

void line_separator_geojson(Rcpp::String& geojson, int i, int n) {
	if (i < (n - 1) ) {
		geojson += "],[";
	}
}

void polygon_separator_geojson(Rcpp::String& geojson, int i, int n) {
	if (i < (n - 1) ) {
		geojson += "]],[[";
	}
}

void add_lonlat_to_geojson(Rcpp::String& geojson, Rcpp::NumericVector& points) {

	Rcpp::Rcout << "adding lonlat" << std::endl;
	Rcpp::Rcout << points << std::endl;

	points.attr("dim") = Dimension(points.size() / 2, 2);
	Rcpp::NumericMatrix m = as< Rcpp::NumericMatrix >(points);

	for (int i = 0; i < m.nrow(); i++) {
		geojson += "[";
		geojson += m(i,0);
		geojson += ",";
		geojson += m(i,1);
		geojson += "]";
		coord_separator(geojson, i, m.nrow());
	}
}

void point_to_geojson(Rcpp::String& geojson, Rcpp::NumericVector& point) {
	Rcpp::Rcout << "adding points" << std::endl;
//	Rcpp::NumericVector points = as<Rcpp::NumericVector>(sfg);
//	Rcpp::Rcout << points << std::endl;
	add_lonlat_to_geojson(geojson, point);
}

void multi_point_to_geojson(Rcpp::String& geojson, Rcpp::NumericVector& points) {

  for (int i = 0; i < points.size(); i++) {
  	Rcpp::Rcout << "adding multipoint" << std::endl;
  	Rcpp::NumericVector point = points[i];
//  	Rcpp::List sfgi = sfg[i];
    add_lonlat_to_geojson(geojson, point);
  }
}

void line_string_to_geojson(Rcpp::String& geojson, Rcpp::NumericVector& line) {
	Rcpp::Rcout << "adding line string" << std::endl;
	Rcpp::Rcout << line << std::endl;
//	Rcpp::List line = as<Rcpp::NumericVector>(sfg);
//	add_lonlat_to_geojson(geojson, line);
  add_lonlat_to_geojson(geojson, line);
}

void multi_line_string_to_geojson(Rcpp::String& geojson, Rcpp::List& sfg) {
	for (int i = 0; i < sfg.size(); i++) {
		Rcpp::Rcout << "adding multi line" << std::endl;
		Rcpp::NumericVector sfgi = sfg[i];
		add_lonlat_to_geojson(geojson, sfgi);
		line_separator_geojson(geojson, i, sfg.size());
	}
}

void polygon_to_geojson(Rcpp::String& geojson, Rcpp::List& sfg) {
	for (int i = 0; i < sfg.size(); i++) {
		Rcpp::Rcout << "adding polygon" << std::endl;
		Rcpp::NumericVector sfgi = sfg[i];
		add_lonlat_to_geojson(geojson, sfgi);
		line_separator_geojson(geojson, i, sfg.size());
	}
}

void multi_polygon_to_geojson(Rcpp::String& geojson, Rcpp::List& sfg) {
	for (int i = 0; i < sfg.size(); i++) {
		Rcpp::Rcout << "adding multi polygon" << std::endl;
		Rcpp::List sfgi = sfg[i];
		polygon_to_geojson(geojson, sfgi);
		polygon_separator_geojson(geojson, i, sfg.size());
	}
}

// need to keep track of lines & polygons
// so we can correctly insert the inner braces ],[
void fetch_coordinates(Rcpp::String& geojson, Rcpp::List& sfc, int& object_counter) {

	Rcpp::CharacterVector cls;
	std::string geom_type;

	Rcpp::Rcout << "sfc size: " << sfc.size() << std::endl;

	for (Rcpp::List::iterator it = sfc.begin(); it != sfc.end(); it++) {

		Rcpp::Rcout << "object counter " << object_counter << std::endl;

		//if (object_counter > 0) {
    //    geojson += ",";
		//}
		//line_separator_geojson(geojson, object_counter, sfc.size());

		switch( TYPEOF(*it) ) {
		case VECSXP: {
			Rcpp::List tmp = as<Rcpp::List>(*it);
			if(!Rf_isNull(tmp.attr("class"))) {

				cls = getSfClass(tmp);
				geom_type = cls[1];    // TODO: error handle (there should aways be 3 elements as we're workgin wtih sfg objects)
				begin_geojson_geometry(geojson, tmp, geom_type);
				fetch_coordinates(geojson, tmp, object_counter);
				end_geojson_geometry(geojson, geom_type);
			} else {
				// if no class attribute, go further into the list to try and find one
				fetch_coordinates(geojson, tmp, object_counter);
			}
			//line_separator_geojson(geojson, object_counter, sfc.size());
			object_counter++;
			break;
		}
		case REALSXP: {
			Rcpp::NumericVector tmp = as<Rcpp::NumericVector>(*it);
			if(!Rf_isNull(tmp.attr("class"))) {

				cls = getSfClass(tmp);
				geom_type = cls[1];
				begin_geojson_geometry(geojson, geom_type);
				add_lonlat_to_geojson(geojson, tmp);
				end_geojson_geometry(geojson, geom_type);
			} else {
				add_lonlat_to_geojson(geojson, tmp);
			}
			//line_separator_geojson(geojson, object_counter, sfc.size());
			object_counter++;
			break;
		}
		case INTSXP: {
			break;
		}
		default: {
			Rcpp::stop("Coordinates could not be found");
		}
		}
	}
}

Rcpp::String add_geometry_to_stream(Rcpp::List& sfg) {

  Rcpp::String geojson;
	int object_counter = 0;

//	fetch_coordinates(geojson, sfg, object_counter);

	return geojson;
}

// if only one object with properties, it's a 'feature'
// if only one object without properties, it's a 'geometry'
// if many objects it's a 'featurecollection'

rapidjson::Value get_json_value(SEXP s, rapidjson::Document::AllocatorType& allocator) {
	// TODO:
	// switch on R type and return the rapidjson equivalent
	rapidjson::Value v1;
	switch (TYPEOF(s)) {
	case VECSXP: {
		rapidjson::Value v(rapidjson::kStringType);
		std::string st = as< std::string >(s);
		v.SetString(st.c_str(), allocator);
		return v;
		}
	default: {
		rapidjson::Value v(rapidjson::kStringType);
		std::string st = as< std::string >(s);
		v.SetString(st.c_str(), allocator);
		return v;
	}
	}
	return v1;
}


void vector_to_json(Rcpp::StringVector& sv, std::string& this_type, std::string& this_name) {
	std::string this_value;

	if (this_type == "Number") {
		for (int j = 0; j < sv.size(); j++) {
			this_value = sv[j];
			if(this_value == "NA") {
				this_value = "null";
			}
			sv[j] = "\"" + this_name + "\"" + ":" + this_value;
		}
	} else if (this_type == "Logical") {
    for (int j = 0; j < sv.size(); j++) {
      this_value = sv[j];
    	if(this_value == "NA") {
    		this_value = "null";
    	}
	  	transform(this_value.begin(), this_value.end(), this_value.begin(), tolower);
	  	sv[j] = "\"" + this_name + "\"" + ":" + this_value;
    }
	} else {
		for (int j = 0; j < sv.size(); j++) {
			this_value = sv[j];
			if (this_value == "NA") {
				this_value = "null";
			} else {
				this_value = "\"" + this_value + "\"";
			}
			sv[j] = "\"" + this_name + "\"" + ":" + this_value;
		}
	}
}

void write_geojson(Rcpp::String& geojson, SEXP sfg, std::string& geom_type, Rcpp::CharacterVector& cls) {

	//geometry_json[i] = add_geometry_to_stream(sfg);
	if (geom_type == "POINT") {

		Rcpp::NumericVector point = as<Rcpp::NumericVector>(sfg);
		point_to_geojson(geojson, point);
	} else if (geom_type == "MULTIPIONT") {

		Rcpp::NumericVector multipoint = as<Rcpp::NumericVector>(sfg);
		multi_point_to_geojson(geojson, multipoint);
	} else if (geom_type == "LINESTRING") {

		Rcpp::NumericVector line = as<Rcpp::NumericVector>(sfg);
		line_string_to_geojson(geojson, line);
	} else if (geom_type == "MULTILINESTRING") {

		Rcpp::List multiline = as<Rcpp::List>(sfg);
		multi_line_string_to_geojson(geojson, multiline);
	} else if (geom_type == "POLYGON") {

		Rcpp::List polygon = as<Rcpp::List>(sfg);
		polygon_to_geojson(geojson, polygon);
	} else if (geom_type == "MULTIPOLYGON") {

		Rcpp::List multipolygon = as<Rcpp::List>(sfg);
		multi_polygon_to_geojson(geojson, multipolygon);
	} else if (geom_type == "GEOMETRYCOLLECTION") {

		Rcpp::List gc = as<Rcpp::List>(sfg);
		for (int i = 0; i < gc.size(); i++) {
			Rcpp::Rcout << "gc" << std::endl;
			Rcpp::List sfgi = gc[i];

			make_gc_type(geojson, gc, geom_type, cls);
			//write_geometry(sfgi, geojson);
			//write_geojson(geojson, sfgi, geom_type, cls);
		}
	}
}

void make_gc_type(Rcpp::String& geojson, Rcpp::List& sfg, std::string& geom_type, Rcpp::CharacterVector& cls) {

	for (Rcpp::List::iterator it = sfg.begin(); it != sfg.end(); it++) {
		Rcpp::Rcout << "finding type" << std::endl;
		switch( TYPEOF(*it) ) {
		case VECSXP: {
			Rcpp::List tmp = as<Rcpp::List>(*it);
			if (!Rf_isNull(tmp.attr("class"))) {
				Rcpp::Rcout << "found type (list) " << std::endl;
				cls = tmp.attr("class");
				Rcpp::Rcout << cls << std::endl;
				geom_type = cls[1];    // TODO: error handle (there should aways be 3 elements as we're workgin wtih sfg objects)\
				write_geojson(geojson, tmp, geom_type, cls);
			} else {
				make_gc_type(geojson, tmp, geom_type, cls);
			}
			break;
		}
		case REALSXP: {
			Rcpp::NumericVector tmp = as<Rcpp::NumericVector>(*it);
			if (!Rf_isNull(tmp.attr("class"))) {
				Rcpp::Rcout << "found type (num vec) " << std::endl;
				cls = tmp.attr("class");
				Rcpp::Rcout << "type: " << cls << std::endl;
				geom_type = cls[1];
				write_geojson(geojson, tmp, geom_type, cls);
			}
			break;
		}
		case INTSXP: {
			break;
		}
		default: {
			Rcpp::stop("Coordinates could not be found");
		}
		}
	}
}

void write_geometry(Rcpp::List& sfg, Rcpp::String& geojson) {
	Rcpp::Rcout << "write_geometry" << std::endl;
	Rcpp::CharacterVector cls = getSfClass(sfg);
	//Rcpp::CharacterVector cls;
	//std::string geom_type;

	Rcpp::Rcout << "cls : " << cls << std::endl;
	Rcpp::String g = cls[1];
	std::string geom_type = g;
	begin_geojson_geometry(geojson, geom_type);

	write_geojson(geojson, sfg, geom_type, cls);

	end_geojson_geometry(geojson, geom_type);
}

void geometry_vector_to_geojson(Rcpp::StringVector& geometry_json, Rcpp::List& sfc) {

	Rcpp::List sfg;
	Rcpp::String geojson;
  for (int i = 0; i < sfc.size(); i++) {
  	sfg = sfc[i];
  	//Rcpp::CharacterVector cls = getSfClass(sfg);
  	//Rcpp::Rcout << "cls : " << cls << std::endl;
  	//Rcpp::String g = cls[1];
  	//std::string geom_type = g;
  	//begin_geojson_geometry(geojson, geom_type);
  	//write_geojson(geojson, sfg, geom_type);
  	//end_geojson_geometry(geojson, geom_type);
  	write_geometry(sfg, geojson);
  	geometry_json[i] = geojson;
  }

}

Rcpp::String matrix_row_to_json(Rcpp::StringMatrix& json_mat, int i) {
	std::ostringstream os;
	int n = json_mat.ncol();
	os << "{\"type\":\"Feature\",\"properties\":{";
	for (int j = 0; j < (n-1); j++) {
		os << json_mat(i, j);
		coord_separator(os, j, (n-1));
	}
	os << "},";

	os << "\"geometry\":";
	//s = json_mat(i, (n-1));
	//st = s;
	os << json_mat(i, (n-1));
	os << "}";

	Rcpp::String res = os.str();
	return res;
}

// [[Rcpp::export]]
Rcpp::StringVector rcpp_sf_to_geojson(Rcpp::List sf) {

	//std::ostringstream os;
	Rcpp::List sf_copy = clone(sf);
	// If it contains properties
	// it's a 'feature' (or featureCollection)
	//
	// if 'atomise', return one object per row

	Rcpp::StringVector column_types(sf_copy.size() - 1);
	//get_column_type(sf, column_types);
	Rcpp::StringVector property_names(sf_copy.size() - 1);

	std::string geom_column = sf_copy.attr("sf_column");
	Rcpp::StringVector col_names = sf_copy.names();

	// fill 'property_names' with all the columns which aren't 'sf_column'
	int property_counter = 0;
	for (int i = 0; i < sf.length(); i++) {
		if (col_names[i] != geom_column) {
			property_names[property_counter] = col_names[i];
			property_counter++;
		}
	}

	get_column_type(sf_copy, property_names, column_types);
	Rcpp::List sfc = sf_copy[geom_column];
	Rcpp::List properties;

	// TODO:
	// construct a StringMatrix with the dimensions of sf
	// then I can fill a column at a time with a string of JSON...
	// then can manipulate it as I want at the end; either atomising or combining
	Rcpp::StringMatrix json_mat(sfc.length(), col_names.size()); // row x cols
	std::string this_name;
	std::string this_type;
	std::string this_value;
	Rcpp::StringVector this_vector;

	for (int i = 0; i < property_names.length(); i++) {
		// iterate the list elements
		this_name = property_names[i];
		this_type = column_types[i];
		this_vector = as< Rcpp::StringVector >(sf_copy[this_name]);

		vector_to_json(this_vector, this_type, this_name);
		//this_vector = "{\"properties\":{" + this_vector + "}";
		json_mat(_, i) = this_vector;

		// TODO: what if there's a mssing element?
	}

	Rcpp::StringVector geometry_json(sfc.length());
	geometry_vector_to_geojson(geometry_json, sfc);
	json_mat(_, (json_mat.ncol() - 1) ) = geometry_json;

	Rcpp::StringVector res(json_mat.nrow());

	// If properties, do this bit. else return a vector (column of matrix)
	for (int i = 0; i < res.length(); i++) {
		res[i] = matrix_row_to_json(json_mat, i);
	}

	return res;
}


/// DESIGN:
/// should work for both properties & non-property sf
// result vector res(nrow(sf));
// property_columns();
// property_column_types();
// for (i = 0; i < nrow(sf); i++) {
//   for (j = 0; j < ncol(properties); j++){
//      stream each column into "properties": { }
//   }
//   if (ncol(properties) > 0) {
//      needs to be a {"type":"Feature", ...}
//   then create "geometry":{"type" :"geom","coordinates":[]}
// }
// result will be a vector of JSON.
// can combine or keep 'atomic'.
