<?xml version="1.0"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xsl:template match="element" name="pointTemplate">
    <xsl:variable name="label" as="xs:string" select="./@label"/>
    <xsl:variable name="x" as="xs:double" select="./coords/@x"/>
    <xsl:variable name="y" as="xs:double" select="./coords/@y"/>
    <FixedPoint label="{$label}">
      <Double value="{$x}"/>
      <Double value="{$y}"/>
    </FixedPoint>
  </xsl:template>

  <xsl:template match="element" name="segmentTemplate">
    <xsl:variable name="label" as="xs:string" select="./output/@a0"/>
    <xsl:variable name="A" as="xs:string" select="./input/@a0"/>
    <xsl:variable name="B" as="xs:string" select="./input/@a1"/>
    <SegmentAB label="{$label}">
      <Parent label="{$A}"/>
      <Parent label="{$B}"/>
    </SegmentAB>
  </xsl:template>

  <xsl:template match="/geogebra/construction">
    <Kig>
      <xsl:for-each select="./*">
	<xsl:choose>
          <xsl:when test="(name(.) = 'element') and (./@type = 'point')">
            <xsl:call-template name="pointTemplate"/>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Segment')">
            <xsl:call-template name="segmentTemplate"/>
          </xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </Kig>
  </xsl:template>
</xsl:stylesheet>
