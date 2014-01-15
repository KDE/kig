<?xml version="1.0"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/geogebra/construction">
    <Kig>
      <FixedPoint label="A">
	<Double value="2.0"/>
	<Double value="1.0"/>
      </FixedPoint>
      <FixedPoint label="B">
	<Double value="4.0"/>
	<Double value="2.0"/>
      </FixedPoint>
      <SegmentAB label="a">
	<Parent label="A"/>
	<Parent label="B"/>
      </SegmentAB>
    </Kig>
  </xsl:template>
</xsl:stylesheet>
