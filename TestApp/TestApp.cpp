/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2025 Julian Williams
 *
 * JawEngine 0.2.0
 * https://github.com/Eidolon2003/JawEngine
 */

#include "../jawengine/JawEngine.h"
#include "../jawengine/libs/profile.h"

const char *abc = "\
%abc																				\n\
X:1																					\n\
T:Mario																				\n\
V:1 jaw-waveform=square jaw-amplitude=5000											\n\
V:2 jaw-waveform=square jaw-amplitude=5000											\n\
V:3 jaw-waveform=triangle jaw-amplitude=10000										\n\
L:1/8																				\n\
Q:1/4=200																			\n\
M:4/4																				\n\
K:C																					\n\
%																					\n\
V:1																					\n\
e.exexLcex | .g2x2.G2x2	|:															\n\
|: .c2x.G2x.E2 | x.A2B2_B.A2 | G4/3e4/3g4/3.a2fg | x.e2cd.B2x ::					\n\
V:2																					\n\
^F.^Fx^FxL^F^Fx | .B2x2.G2x2 |:														\n\
|: .E2x.C2x.G,2 | x.C2D2_D.C2 | C4/3G4/3B4/3.c2AB | x.A2EF.D2x ::					\n\
V:3																					\n\
D,.D,xD,xLD,D,x | .G2x2.G,2x2 |:													\n\
|: .G,2x.E,2x.C,2 | x.F,2G,2_G,.F,2 | E,4/3C4/3E4/3.F2DE | x.C2A,B,.G,2x ::			\n\
%																					\n\
V:1																					\n\
x2g_gf.^dxe | x^GAcxAcd | x2g_gf.^dxe | x.c'2c'.c'2x2 |								\n\
V:2																					\n\
x2e_ed.Bxc | xEFGxCEF | x2e_ed.Bxc | x.g2g.g2x2 |									\n\
V:3																					\n\
.C,2xG,x2.C2 | .F,2xC.C2.F,2 | .C,2xE,x2G,C | x.f2f.f2.G,2 |						\n\
%																					\n\
V:1																					\n\
x2g_gf.^dxe | x^GAcxAcd | x2._e2xdx2 | cx7 :|										\n\
V:2																					\n\
x2e_ed.Bxc | xEFGxCEF | x2._A2xFx2 | Ex7 :|											\n\
V:3																					\n\
.C,2xG,x2.C2 | .F,2xC.C2.F,2 | .C,2._A,2x_B,x2 | .C2xG,.G,2.C,2 :|					\n\
%																					\n\
V:1																					\n\
c.cxcxLcdx | ecxAGx3 | c.cxcxLcde | x8 |											\n\
V:2																					\n\
_A._Ax_AxL_A_Bx | GExECx3 | _A._Ax_AxL_A_BG | x8 |									\n\
V:3																					\n\
._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 | ._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 |			\n\
%																					\n\
V:1																					\n\
c.cxcxLcdx | ecxAGx3 | e.exexLcex | .g2x2.G2x2 |									\n\
V:2																					\n\
_A._Ax_AxL_A_Bx | GExECx3 | ^F.^Fx^FxL^F^Fx | .B2x2.G2x2 |							\n\
V:3																					\n\
._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 | D,.D,xD,xLD,D,x | .G2x2.G,2x2 |				\n\
%																					\n\
V:1																					\n\
|: .c2x.G2x.E2 | x.A2B2_B.A2 | G4/3e4/3g4/3.a2fg | x.e2cd.B2x :|:					\n\
V:2																					\n\
|: .E2x.C2x.G,2 | x.C2D2_D.C2 | C4/3G4/3B4/3.c2AB | x.A2EF.D2x :|:					\n\
V:3																					\n\
|: .G,2x.E,2x.C,2 | x.F,2G,2_G,.F,2 | E,4/3C4/3E4/3.F2DE | x.C2A,B,.G,2x :|:		\n\
%																					\n\
V:1																					\n\
ecxGx2.^G2 | Afxf.A2x2 | B4/3.a4/3.a4/3 a4/3.g4/3.f4/3 | ecxA.G2x2 |				\n\
V:2																					\n\
cAxEx2.E2 | Fcxc.F2x2 | G4/3.f4/3.f4/3 f4/3.e4/3.d4/3 | cAxF.E2x2 |					\n\
V:3																					\n\
.C,2xL^F,.G,2.C2 | .F,2.F,2CC.F,2 | .D,2xLF,.G,2.B,2 | .G,2.G,2.C.C.G,2				\n\
%																					\n\
V:1																					\n\
ecxGx2.^G2 | Afxf.A2x2 | Bfxf f4/3e4/3d4/3 | cExE.C2x2 :|							\n\
V:2																					\n\
cAxEx2.E2 | Fcxc.F2x2 | Gdxd d4/3c4/3B4/3 | GExE.C2x2 :|							\n\
V:3																					\n\
.C,2xL^F,.G,2.C2 | .F,2.F,2CC.F,2 | .G,2xG, G,4/3A,4/3B,4/3 | .C2.G,2.C,2x2 :| 		\n\
%																					\n\
V:1																					\n\
c.cxcxLcdx | ecxAGx3 | c.cxcxLcde | x8 |											\n\
V:2																					\n\
_A._Ax_AxL_A_Bx | GExECx3 | _A._Ax_AxL_A_BG | x8 |									\n\
V:3																					\n\
._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 | ._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 |			\n\
%																					\n\
V:1																					\n\
c.cxcxLcdx | ecxAGx3 | e.exexLcex | .g2x2.G2x2 |									\n\
V:2																					\n\
_A._Ax_AxL_A_Bx | GExECx3 | ^F.^Fx^FxL^F^Fx | .B2x2.G2x2 |							\n\
V:3																					\n\
._A,,2x_E,x2._A,2 | .G,2xC,x2.G,,2 | D,.D,xD,xLD,D,x | .G2x2.G,2x2 |				\n\
%																					\n\
V:1																					\n\
ecxGx2.^G2 | Afxf.A2x2 | B4/3.a4/3.a4/3 a4/3.g4/3.f4/3 | ecxA.G2x2 |				\n\
V:2																					\n\
cAxEx2.E2 | Fcxc.F2x2 | G4/3.f4/3.f4/3 f4/3.e4/3.d4/3 | cAxF.E2x2 |					\n\
V:3																					\n\
.C,2xL^F,.G,2.C2 | .F,2.F,2CC.F,2 | .D,2xLF,.G,2.B,2 | .G,2.G,2.C.C.G,2				\n\
%																					\n\
V:1																					\n\
ecxGx2.^G2 | Afxf.A2x2 | Bfxf f4/3e4/3d4/3 | cExE.C2x2 :|							\n\
V:2																					\n\
cAxEx2.E2 | Fcxc.F2x2 | Gdxd d4/3c4/3B4/3 | GExE.C2x2 :|							\n\
V:3																					\n\
.C,2xL^F,.G,2.C2 | .F,2.F,2CC.F,2 | .G,2xG, G,4/3A,4/3B,4/3 | .C2.G,2.C,2x2 :| 		\n\
";

static void init(jaw::properties *p) {
	profile::clear();
	profile::begin(0);
	jaw::soundid s = sound::abc(abc, { .loop=false });
	profile::end(0);
	sound::start(s);
}

static void loop(jaw::properties *p) {
	char *buf = util::tempalloc<char>(16);
	snprintf(buf, 16, "%.2f", jaw::to_millis(profile::get(0)));
	draw::enqueue(draw::str{
		.rect = jaw::recti({}, p->size),
		.str = buf,
		.color = jaw::color::WHITE
	}, 0);
}

int main() {
	jaw::properties props;
	props.targetFramerate = 30;
	props.scale = 20;
	props.size.x = 40;
	props.size.y = 30;
	props.title = "ABC";
	engine::start(&props, nullptr, init, loop); 
}