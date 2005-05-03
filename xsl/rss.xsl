<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:rss="http://purl.org/rss/1.0/"
                xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
                xmlns:cvsf="http://nwalsh.com/xslt/ext/com.nwalsh.saxon.CVS"
                xmlns:cvs="http://nwalsh.com/rdf/cvs#"
                xmlns:dc="http://purl.org/dc/elements/1.1/"
                xmlns="http://www.w3.org/1999/xhtml"
                exclude-result-prefixes="rss rdf cvs dc cvsf"
                version="1.0">

<xsl:output method="xml"/>

<xsl:template match="rss">
  <xsl:variable name="rss" select="document(@feed, .)"/>

  <div class='rss'>
    <xsl:choose>
      <xsl:when test="not($rss)">
        <xsl:message>RSS Failed: <xsl:value-of select="@feed"/></xsl:message>
        <xsl:text>[ RSS Failed: </xsl:text>
        <xsl:value-of select="@feed"/>
	<xsl:text> ]</xsl:text>
      </xsl:when>
      <xsl:when test="$rss/rdf:RDF">
        <xsl:apply-templates select="$rss/*/rss:channel"/>
      </xsl:when>
      <xsl:otherwise>
        <!-- is there an otherwise case? -->
        <xsl:apply-templates select="$rss//rss:channel"/>
      </xsl:otherwise>
    </xsl:choose>
  </div>
</xsl:template>

<xsl:template match="rss:channel">
  <xsl:variable name="image-resource" select="rss:image/@rdf:resource"/>
  <xsl:variable name="image" select="//rss:image[@rdf:about = $image-resource]"/>

  <xsl:if test="$image">
    <xsl:choose>
      <xsl:when test="$image/rss:link">
        <a href="{$image/rss:link}">
          <img src="{$image/rss:url}" alt="{$image/rss:title}"/>
        </a>
      </xsl:when>
      <xsl:otherwise>
        <img src="{$image/rss:url}" alt="{$image/rss:title}"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

<!--  not used for CVS commits feed
  <xsl:apply-templates select="rss:title"/>
  <xsl:apply-templates select="rss:description"/>
 -->
  <xsl:apply-templates select="rss:items"/>

</xsl:template>

<xsl:template match="rss:title">
  <xsl:param name="wrapper" select="'h3'"/>
  <xsl:element name="{$wrapper}">
    <xsl:choose>
      <xsl:when test="../rss:link">
        <a href="{../rss:link[1]}">
          <xsl:apply-templates/>
        </a>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

<xsl:template match="rss:description">
    <div class='rss_entry'>
    <xsl:if test="../dc:date|../cvs:date">
      <span class="date">
        <xsl:choose>
          <xsl:when test="../dc:date">
            <xsl:variable name="timeString" select="../dc:date[1]"/>
            <xsl:value-of select="substring($timeString, 1, 10)"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="substring($timeString, 12, 5)"/>
            <xsl:text> UTC</xsl:text>
          </xsl:when>
          <xsl:when test="function-available('cvsf:localTime')">
            <xsl:variable name="timeString" select="cvsf:localTime(../cvs:date[1])"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="substring($timeString, 1, 3)"/>
            <xsl:text>, </xsl:text>
            <xsl:value-of select="substring($timeString, 9, 2)"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="substring($timeString, 5, 3)"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="substring($timeString, 25, 4)"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="../cvs:date[1]"/>
          </xsl:otherwise>
	</xsl:choose>
      </span>
    </xsl:if>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="rss:items">
  <xsl:for-each select="rdf:Seq/rdf:li[@resource and @resource != '']">
    <xsl:variable name="resource" select="@resource"/>
    <xsl:variable name="item" select="//rss:item[@rdf:about = $resource]"/>
    <xsl:if test="not($item)">
      <xsl:message>
        <xsl:text>RSS Warning: there is no item labelled: </xsl:text>
        <xsl:value-of select="$resource"/>
      </xsl:message>
    </xsl:if>
    <xsl:if test="count($item) &gt; 1">
      <xsl:message>
        <xsl:text>RSS Warning: there is more than one item labelled: </xsl:text>
        <xsl:value-of select="$resource"/>
      </xsl:message>
    </xsl:if>
    <xsl:apply-templates select="$item"/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="rss:item">
  <xsl:message>RSS item: <xsl:value-of select="rss:title"/></xsl:message>
  <xsl:apply-templates select="rss:description"/>
</xsl:template>

<!-- Unescape markup  -->
<xsl:template match="text()">
  <xsl:value-of disable-output-escaping="yes" select="."/>
</xsl:template>

</xsl:stylesheet>
