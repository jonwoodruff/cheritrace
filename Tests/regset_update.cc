#include "streamtrace.hh"
#include <assert.h>

#include <stdio.h>

using cheri::streamtrace::capability_register;

#define REG_MAGIC 0xa5ea51decafe0000

static void expect_gpr(int reg, uint64_t val)
{
	assert(val == (REG_MAGIC | reg));
}

static void expect_cap(int reg, const capability_register &cap)
{
	uint64_t off = (REG_MAGIC | reg);
	assert(cap.base == 0x00);
	assert(cap.offset == off);
	assert(cap.length == 0xffffffffffffffff);
	assert(cap.permissions == 0xffff);
	assert(cap.type == 0x80000000);
	assert(cap.valid);
	/* XXXAM: there is some confusion about the sealed bit (see streamtrace.cc) */
	/* assert(cap.unsealed); */
}

int main()
{
	/* Check that the register set is updated for each register */
	
	auto trace = cheri::streamtrace::trace::open(SOURCE_PATH "/regset_update.trace");
	assert(trace);
	bool success = trace->seek_to(trace->size() - 1);	
	assert(success);	
	auto regs = trace->get_regs();
	for (int i = 0; i < 32; i++)
	{
		/* $zero is not stored for GPRs, indexed from 1 */
		if (i > 0)
		{
			expect_gpr(i, regs.gpr[i - 1]);
			assert(regs.valid_gprs[i - 1]);
		}
		expect_cap(i, regs.cap_reg[i]);
		assert(regs.valid_caps[i]);
	}
	return 0;
}
