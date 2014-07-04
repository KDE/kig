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
    <!-- The parameter - 'ObjectType' passed to this template *must* only be the 'fullTypeName' of some valid Kig
    ObjectType. Passing self-chosen names as parameter will not recognise the Object (and it will not draw..) -->
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
            <xsl:variable name="labelInput1" select="./input/@a0"/>
            <xsl:variable name="labelInput2" select="./input/@a1"/>
            <xsl:variable name="typeInput2" select="../command[output/@a0=current()/$labelInput1]/@name"/>
            <xsl:variable name="typeInput1" select="../command[output/@a0=current()/$labelInput2]/@name"/>
            <xsl:if test="($typeInput1='Line' or $typeInput2='Line')">
              <xsl:call-template name="argsTemplate">
                <xsl:with-param name="objectType" select="'LineParallel'"/>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="not($typeInput1='Line' or $typeInput2='Line')">
              <xsl:call-template name="argsTemplate">
                <xsl:with-param name="objectType" select="'LineAB'"/>
              </xsl:call-template>
            </xsl:if>
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
          <xsl:when test="(name(.) = 'command') and (./@name = 'PolyLine')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'OpenPolygon'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Vector')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'Vector'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Polygon')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'PolygonBNP'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Circle')">
            <xsl:choose>
              <xsl:when test="count(./input/@*) = 2">
                <xsl:variable name="labelInput1" select="./input/@a0"/>
                <xsl:variable name="labelInput2" select="./input/@a1"/>
                <xsl:variable name="typeInput1" select="../element[@label=current()/$labelInput1]/@type"/>
                <xsl:variable name="typeInput2" select="../element[@label=current()/$labelInput2]/@type"/>
                <!-- 
                     These tests separate geogebra's circle-center-point type from compass and circle-center-radius types
                     if both the inputs are of point type then the circle is of circle-center-point type (CircleBCPType),
                     otherwise (CircleBPRType).
                -->
                <xsl:if test="not($typeInput1='point' and $typeInput2='point')">
                  <xsl:call-template name="argsTemplate">
                    <xsl:with-param name="objectType" select="'CircleBPR'"/>
                  </xsl:call-template>
                </xsl:if>
                <xsl:if test="($typeInput1='point' and $typeInput2='point')">
                  <xsl:call-template name="argsTemplate">
                    <xsl:with-param name="objectType" select="'CircleBCP'"/>
                  </xsl:call-template>
                </xsl:if>
              </xsl:when>
              <xsl:when test="count(./input/@*) = 3">
                <xsl:call-template name="argsTemplate">
                  <xsl:with-param name="objectType" select="'CircleBTP'"/>
                </xsl:call-template>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'CircumcircleArc')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'ArcBTP'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Parabola')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'ParabolaBDP'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Ellipse')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'EllipseBFFP'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Hyperbola')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'HyperbolaBFFP'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Conic')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'ConicB5P'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Mirror')">
            <xsl:variable name="labelReflector" select="./input/@a1"/>
            <xsl:variable name="typeReflector" select="../command[output/@a0=current()/$labelReflector]/@name"/>
            <xsl:choose>
              <xsl:when test="($typeReflector='Line')">
                <xsl:call-template name="argsTemplate">
                  <xsl:with-param name="objectType" select="'LineReflection'"/>
                </xsl:call-template>
              </xsl:when>
              <xsl:when test="($typeReflector='Circle')">
                <!--TODO It cannot open reflection of Polygons. Make some arrangement to open
                reflections of polygons-->
                <xsl:call-template name="argsTemplate">
                  <xsl:with-param name="objectType" select="'CircularInversion'"/>
                </xsl:call-template>
              </xsl:when>
              <!-- The following test should have been for null string but it is not working here
              ( Line, Circle or null are the only possible values of typeReflector variable. )
              -->
              <xsl:when test="not($typeReflector='Line' or $typeReflector='Circle')">
                <xsl:call-template name="argsTemplate">
                  <xsl:with-param name="objectType" select="'PointReflection'"/>
                </xsl:call-template>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Translate')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'Translation'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Dilate')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'ScalingOverCenter'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Polar')">
            <xsl:call-template name="argsTemplate">
              <xsl:with-param name="objectType" select="'ConicPolarLine'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="(name(.) = 'command') and (./@name = 'Diameter')">
            <!-- Kig can't draw diameters of conics ( ?? )-->
          </xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </Kig>
  </xsl:template>
</xsl:stylesheet>
