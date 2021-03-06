

#geo_melbourne <- paste0( googleway::geo_melbourne, collapse = "")
# sf <- geojson_sf(geo_melbourne)
#
# coords <- as.data.frame(matrix(unlist(st_geometry(sf[22, ])), ncol = 2))
# coords$geo <- paste0('[', coords[,1], ',', coords[,2], ']')
# coords <- paste0('[[', paste0(coords$geo, collapse = ","), ']]')
# coords <- paste0('"geometry":{"type":"Polygon","coordinates":',coords,'}')
#
# sf::st_geometry(sf) <- NULL
#
# sf <- sf[22, ]
# js <- jsonlite::toJSON(sf)
# js <- paste0('"properties":',gsub('\\[|\\]','',js),'')
#
# geo_melbourne <- paste0('{"type":"Feature",',js, ',',coords,'}')
# sf <- geojson_sf(geo_melbourne)


# attr( geo_melbourne, "class") <- "json"
# usethis::use_data(geo_melbourne, overwrite = T)
#
# library(sf)
# nc <- st_read(system.file("shape/nc.shp", package="sf"))
#
# geo <- geojsonsf::sf_geojson( nc )
#
# ## v1.1
# format( object.size( geo ), units = "Kb" )
# # [1] "84.1 kb"
#
# ## v1.2
# format( object.size( geo ), units = "Kb" )
# # [1] "124.8 Kb"

attr( geo_melbourne, "class") <- c("geojson", "json")
usethis::use_data(geo_melbourne, overwrite = T)
