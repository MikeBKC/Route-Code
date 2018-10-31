<?xml version='1.0'?>
<!--############################################################################# 
|	$Id: slidestoc.mod.xsl,v 1.2 2013/11/11 10:19:32 chen.kun Exp $
|- #############################################################################
|	$Author: chen.kun $
|														
|   PURPOSE:
+ ############################################################################## -->

<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
    exclude-result-prefixes="doc" version='1.0'>


    <!--############################################################################# -->
    <!-- DOCUMENTATION                                                                -->
    <doc:reference id="slides" xmlns="">
    </doc:reference>
    <!--############################################################################# -->


<xsl:template match="slides" mode="toc">
<xsl:call-template name="label.id"/>
</xsl:template>

<xsl:template match="title|titleabbrev" mode="toc">
  <xsl:apply-templates mode="toc"/>
</xsl:template>

<xsl:template match="speakernotes" mode="toc">
</xsl:template>

</xsl:stylesheet>