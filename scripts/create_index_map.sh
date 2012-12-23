#!/bin/sh

MAP_FILE="sessions_index.kml"

function file_header()
{
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  echo "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"
  echo "<Document>"
  echo "<name>Index of sessions</name>"
  echo "<open>0</open>"
  echo "<Style id=\"kalenji_runner\">"
  echo "<IconStyle>"
  echo "<color>ff0000ff</color>"
  echo "<scale>1.0</scale>"
  echo "<Icon>"
  echo "<href>http://maps.google.com/mapfiles/kml/pal2/icon57.png</href>"
  echo "</Icon>"
  echo "</IconStyle>"
  echo "</Style>"
}

function file_footer()
{
  echo "</Document>"
  echo "</kml>"
}

touch $MAP_FILE
file_header > $MAP_FILE
for file in `find . -name \*.kml`
do
    grep -B 1 -A 5 Runner $file | grep -v "^--$" | sed "s,Runner,$file," >> $MAP_FILE
done
file_footer >> $MAP_FILE
