/*
 ******************************************************************
 * COPYRIGHT � Symphony Teleca
 *----------------------------------------------------------------*
 * MODULE : AEM - AVDECC Clock Domain Descriptor
 *
 * PROGRAMMER : Ken Carlino (Triple Play Integration)
 * DATE :       26-Dec-2013
 * VERSION :    1.0
 *
 *----------------------------------------------------------------*
 *
 * MODULE SUMMARY : Implements the AVDECC Clock Domain IEEE Std 1722.1-2013 clause 7.2.32 
 *
 *----------------------------------------------------------------*
 *
 * MODIFICATION RECORDS
 *
 ******************************************************************
 */

#include <stdlib.h>

#define	AVB_LOG_COMPONENT	"AEM"
#include "openavb_log.h"

#include "openavb_rawsock.h"
#include "openavb_aem.h"
#include "openavb_descriptor_clock_domain.h"


////////////////////////////////
// Private (internal) functions
////////////////////////////////
openavbRC openavbAemDescriptorClockDomainToBuf(void *pVoidDescriptor, U16 bufLength, U8 *pBuf, U16 *descriptorSize)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_clock_domain_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor || !pBuf || !descriptorSize) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	if (bufLength < OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_BASE_LENGTH + (pDescriptor->clock_sources_count * OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_CLOCK_SOURCE_LENGTH)) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVBAVDECC_RC_BUFFER_TOO_SMALL), AVB_TRACE_AEM);
	}

	*descriptorSize = 0;

	openavb_aem_descriptor_clock_domain_t *pSrc = pDescriptor;
	U8 *pDst = pBuf;

	OCT_D2BHTONS(pDst, pSrc->descriptor_type);
	OCT_D2BHTONS(pDst, pSrc->descriptor_index);
	OCT_D2BMEMCP(pDst, pSrc->object_name);
	BIT_D2BHTONS(pDst, pSrc->localized_description.offset, 3, 0);
	BIT_D2BHTONS(pDst, pSrc->localized_description.index, 0, 2);
	OCT_D2BHTONS(pDst, pSrc->clock_source_index);
	OCT_D2BHTONS(pDst, pSrc->clock_sources_offset);
	OCT_D2BHTONS(pDst, pSrc->clock_sources_count);

	int i1;
	for (i1 = 0; i1 < pSrc->clock_sources_count; i1++) {
		OCT_D2BHTONS(pDst, pSrc->clock_sources[i1]);
	}

	*descriptorSize = pDst - pBuf;

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

openavbRC openavbAemDescriptorClockDomainFromBuf(void *pVoidDescriptor, U16 bufLength, U8 *pBuf)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_clock_domain_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor || !pBuf) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	if (bufLength < OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_BASE_LENGTH) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVBAVDECC_RC_BUFFER_TOO_SMALL), AVB_TRACE_AEM);
	}

	U8 *pSrc = pBuf;
	openavb_aem_descriptor_clock_domain_t *pDst = pDescriptor;

	OCT_B2DNTOHS(pDst->descriptor_type, pSrc);
	OCT_B2DNTOHS(pDst->descriptor_index, pSrc);
	OCT_B2DMEMCP(pDst->object_name, pSrc);
	BIT_B2DNTOHS(pDst->localized_description.offset, pSrc, 0xfff8, 3, 0);
	BIT_B2DNTOHS(pDst->localized_description.index, pSrc, 0x0007, 0, 2);
	OCT_B2DNTOHS(pDst->clock_source_index, pSrc);
	OCT_B2DNTOHS(pDst->clock_sources_offset, pSrc);
	OCT_B2DNTOHS(pDst->clock_sources_count, pSrc);

	if (bufLength < OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_BASE_LENGTH + (pDescriptor->clock_sources_count * OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_CLOCK_SOURCE_LENGTH)) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVBAVDECC_RC_BUFFER_TOO_SMALL), AVB_TRACE_AEM);
	}

	int i1;
	for (i1 = 0; i1 < pDst->clock_sources_count; i1++) {
		OCT_B2DNTOHS(pDst->clock_sources[i1], pSrc);
	}

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

openavbRC openavbAemDescriptorClockDomainUpdate(void *pVoidDescriptor)
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_clock_domain_t *pDescriptor = pVoidDescriptor;

	if (!pDescriptor) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	// AVDECC_TODO - Any updates needed?

	AVB_RC_TRACE_RET(OPENAVB_AVDECC_SUCCESS, AVB_TRACE_AEM);
}

////////////////////////////////
// Public functions
////////////////////////////////
extern DLL_EXPORT openavb_aem_descriptor_clock_domain_t *openavbAemDescriptorClockDomainNew()
{
	AVB_TRACE_ENTRY(AVB_TRACE_AEM);

	openavb_aem_descriptor_clock_domain_t *pDescriptor;

	pDescriptor = malloc(sizeof(*pDescriptor));

	if (!pDescriptor) {
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_OUT_OF_MEMORY));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return NULL;
	}
	memset(pDescriptor, 0, sizeof(*pDescriptor));

	pDescriptor->descriptorPvtPtr = malloc(sizeof(*pDescriptor->descriptorPvtPtr));
	if (!pDescriptor->descriptorPvtPtr) {
		free(pDescriptor);
		pDescriptor = NULL;
		AVB_RC_LOG(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_OUT_OF_MEMORY));
		AVB_TRACE_EXIT(AVB_TRACE_AEM);
		return NULL;
	}

	pDescriptor->descriptorPvtPtr->size = sizeof(openavb_aem_descriptor_clock_domain_t);
	pDescriptor->descriptorPvtPtr->bTopLevel = TRUE;
	pDescriptor->descriptorPvtPtr->toBuf = openavbAemDescriptorClockDomainToBuf;
	pDescriptor->descriptorPvtPtr->fromBuf = openavbAemDescriptorClockDomainFromBuf;
	pDescriptor->descriptorPvtPtr->update = openavbAemDescriptorClockDomainUpdate;

	pDescriptor->descriptor_type = OPENAVB_AEM_DESCRIPTOR_CLOCK_DOMAIN;
	pDescriptor->clock_sources_offset = OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_BASE_LENGTH;

	// Default to no localized strings.
	pDescriptor->localized_description.offset = OPENAVB_AEM_NO_STRING_OFFSET;
	pDescriptor->localized_description.index = OPENAVB_AEM_NO_STRING_INDEX;

	AVB_TRACE_EXIT(AVB_TRACE_AEM);
	return pDescriptor;
}

extern DLL_EXPORT bool openavbAemDescriptorClockDomainInitialize(openavb_aem_descriptor_clock_domain_t *pDescriptor, U16 nConfigIdx, const openavb_avdecc_configuration_cfg_t *pConfig)
{
	int i;
	openavb_aem_descriptor_clock_source_t *pClockSourceDescriptor;

	if (!pDescriptor || !pConfig) {
		AVB_RC_LOG_TRACE_RET(AVB_RC(OPENAVB_AVDECC_FAILURE | OPENAVB_RC_INVALID_ARGUMENT), AVB_TRACE_AEM);
	}

	// Specify a name.
	strcpy((char *)(pDescriptor->object_name), "Clock Domain");

	// Add all the current clocks for this configuration to this domain.
	for (i = 0; i < OPENAVB_DESCRIPTOR_CLOCK_DOMAIN_MAX_CLOCK_SOURCES; ++i) {
		pClockSourceDescriptor =
			(openavb_aem_descriptor_clock_source_t *) openavbAemGetDescriptor(nConfigIdx, OPENAVB_AEM_DESCRIPTOR_CLOCK_SOURCE, i);
		if (pClockSourceDescriptor == NULL) { break; }
		pDescriptor->clock_sources[i] = pClockSourceDescriptor->descriptor_index;
		pDescriptor->clock_sources_count = i + 1;
	}

	return TRUE;
}
