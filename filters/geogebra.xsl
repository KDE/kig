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

  <xsl:template match="element" name="argsTemplate">
    <xsl:param name="objectType" as="xs:string"/>
    <xsl:variable name="label" as="xs:string" select="./output/@a0"/>
    <xsl:variable name="A" as="xs:string" select="./input/@a0"/>
    <xsl:variable name="B" as="xs:string" select="./input/@a1"/>
    <xsl:element name="{$objectType}">
      <xsl:attribute name="label" type="xs:string">
	<xsl:value-of select="$label"/>
      </xsl:attribute>
      <xsl:for-each select="./input/@*">
      	<xsl:variable name="objectLabel" select="string(.)"/>
      	<Parent label="{$objectLabel}"/>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

  <xsl:template match="/geogebra/construction">
    <Kig>
      <xsl:for-each select="./*">
	<xsl:choose>
          <xsl:when test="(name(.) = 'element') and (./@type = 'point')">
            <xsl:call-template name="pointTemplate"/>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Segment')">
            <xsl:call-template name="argsTemplate">
	      <xsl:with-param name="objectType" select="'SegmentAB'"/>
	    </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Line')">
            <xsl:call-template name="argsTemplate">
	      <xsl:with-param name="objectType" select="'LineAB'"/>
	    </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Ray')">
            <xsl:call-template name="argsTemplate">
	      <xsl:with-param name="objectType" select="'RayAB'"/>
	    </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Midpoint')">
            <xsl:call-template name="argsTemplate">
	      <xsl:with-param name="objectType" select="'Midpoint'"/>
	    </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'OrthogonalLine')">
            <xsl:call-template name="argsTemplate">
	      <xsl:with-param name="objectType" select="'LinePerpend'"/>
	    </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Circle')">
	    <xsl:choose>
	      <xsl:when test="count(./input/@*) = 2">
		<xsl:call-template name="argsTemplate">
		  <xsl:with-param name="objectType" select="'CircleBCPType'"/>
		</xsl:call-template>
	      </xsl:when>
	      <xsl:when test="count(./input/@*) = 3">
		<xsl:call-template name="argsTemplate">
		  <xsl:with-param name="objectType" select="'CircleBTPType'"/>
		</xsl:call-template>
	      </xsl:when>
	    </xsl:choose>
          </xsl:when>
	</xsl:choose>
      </xsl:for-each>
    </Kig>
  </xsl:template>
</xsl:stylesheet>
