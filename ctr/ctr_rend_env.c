#include "ctr_rend.h"
#include "gl.h"

void ctr_rend_texenv_comb_alpha(GLuint param) {
	switch (param) {
	case GL_REPLACE:
		ctr_state.texenv_comb_alpha = GPU_REPLACE;
		break;
	case GL_MODULATE:
		ctr_state.texenv_comb_alpha = GPU_MODULATE;
		break;
	case GL_ADD:
		ctr_state.texenv_comb_alpha = GPU_ADD;
		break;
	case GL_INTERPOLATE:
		ctr_state.texenv_comb_alpha = GPU_INTERPOLATE;
		break;
	case GL_SUBTRACT:
		ctr_state.texenv_comb_alpha = GPU_SUBTRACT;
		break;
	case GL_DOT3_RGB:
		ctr_state.texenv_comb_alpha = GPU_DOT3_RGB;
		break;
	}
	ctr_state.dirty_texenv_comb = 1;
}


void ctr_rend_texenv_comb_rgb(GLuint param) {
	switch (param) {
	case GL_REPLACE:
		ctr_state.texenv_comb_rgb = GPU_REPLACE;
		break;
	case GL_MODULATE:
		ctr_state.texenv_comb_rgb = GPU_MODULATE;
		break;
	case GL_ADD:
		ctr_state.texenv_comb_rgb = GPU_ADD;
		break;
	case GL_INTERPOLATE:
		ctr_state.texenv_comb_rgb = GPU_INTERPOLATE;
		break;
	case GL_SUBTRACT:
		ctr_state.texenv_comb_rgb = GPU_SUBTRACT;
		break;
	case GL_DOT3_RGB:
		ctr_state.texenv_comb_rgb = GPU_DOT3_RGB;
		break;
	}
	ctr_state.dirty_texenv_comb = 1;
}

void ctr_rend_texenv_op_alpha(GLenum pname, GLuint param) {
	switch (pname) {
	case GL_OPERAND0_ALPHA:
		switch (param) {
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_alpha0 = GPU_TEVOP_A_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_alpha0 = GPU_TEVOP_A_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	case GL_OPERAND1_ALPHA:
		switch (param) {
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_alpha1 = GPU_TEVOP_A_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_alpha1 = GPU_TEVOP_A_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	case GL_OPERAND2_ALPHA:
		switch (param) {
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_alpha2 = GPU_TEVOP_A_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_alpha2 = GPU_TEVOP_A_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	}
	ctr_state.dirty_texenv_op = 1;
}

void ctr_rend_texenv_op_rgb(GLenum pname, GLuint param) {
	switch (pname) {
	case GL_OPERAND0_RGB:
		switch (param) {
		case GL_SRC_COLOR:
			ctr_state.texenv_op_rgb0 = GPU_TEVOP_RGB_SRC_COLOR;
			break;
		case GL_ONE_MINUS_SRC_COLOR:
			ctr_state.texenv_op_rgb0 = GPU_TEVOP_RGB_ONE_MINUS_SRC_COLOR;
			break;
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_rgb0 = GPU_TEVOP_RGB_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_rgb0 = GPU_TEVOP_RGB_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	case GL_OPERAND1_RGB:
		switch (param) {
		case GL_SRC_COLOR:
			ctr_state.texenv_op_rgb1 = GPU_TEVOP_RGB_SRC_COLOR;
			break;
		case GL_ONE_MINUS_SRC_COLOR:
			ctr_state.texenv_op_rgb1 = GPU_TEVOP_RGB_ONE_MINUS_SRC_COLOR;
			break;
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_rgb1 = GPU_TEVOP_RGB_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_rgb1 = GPU_TEVOP_RGB_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	case GL_OPERAND2_RGB:
		switch (param) {
		case GL_SRC_COLOR:
			ctr_state.texenv_op_rgb2 = GPU_TEVOP_RGB_SRC_COLOR;
			break;
		case GL_ONE_MINUS_SRC_COLOR:
			ctr_state.texenv_op_rgb2 = GPU_TEVOP_RGB_ONE_MINUS_SRC_COLOR;
			break;
		case GL_SRC_ALPHA:
			ctr_state.texenv_op_rgb2 = GPU_TEVOP_RGB_SRC_ALPHA;
			break;
		case GL_ONE_MINUS_SRC_ALPHA:
			ctr_state.texenv_op_rgb2 = GPU_TEVOP_RGB_ONE_MINUS_SRC_ALPHA;
			break;
		}
		break;
	}
	ctr_state.dirty_texenv_op = 1;
}

void ctr_rend_texenv_src_alpha(GLenum pname, GLuint param) {
	switch (pname) {
	case GL_SRC0_ALPHA:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_alpha0 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_alpha0 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_alpha0 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TODO: should this be the current texture???
			ctr_state.texenv_src_alpha0 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_alpha0 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_alpha0 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_alpha0 = GPU_TEXTURE2;
			break;
		}
		break;
	case GL_SRC1_ALPHA:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_alpha1 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_alpha1 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_alpha1 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TOTDO: should this be the current texture???
			ctr_state.texenv_src_alpha1 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_alpha1 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_alpha1 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_alpha1 = GPU_TEXTURE2;
			break;
		}
		break;
	case GL_SRC2_ALPHA:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_alpha2 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_alpha2 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_alpha2 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TOTDO: should this be the current texture???
			ctr_state.texenv_src_alpha2 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_alpha2 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_alpha2 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_alpha2 = GPU_TEXTURE2;
			break;
		}
		break;
	}
	ctr_state.dirty_texenv_src = 1;
}

void ctr_rend_texenv_src_rgb(GLenum pname, GLuint param) {
	switch (pname) {
	case GL_SRC0_RGB:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_rgb0 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_rgb0 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_rgb0 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TODO: should this be the current texture???
			ctr_state.texenv_src_rgb0 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_rgb0 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_rgb0 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_rgb0 = GPU_TEXTURE2;
			break;
		}
		break;
	case GL_SRC1_RGB:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_rgb1 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_rgb1 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_rgb1 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TOTDO: should this be the current texture???
			ctr_state.texenv_src_rgb1 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_rgb1 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_rgb1 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_rgb1 = GPU_TEXTURE2;
			break;
		}
		break;
	case GL_SRC2_RGB:
		switch (param) {
		case GL_PRIMARY_COLOR:
			ctr_state.texenv_src_rgb2 = GPU_PRIMARY_COLOR;
			break;
		case GL_CONSTANT:
			ctr_state.texenv_src_rgb2 = GPU_CONSTANT;
			break;
		case GL_PREVIOUS:
			ctr_state.texenv_src_rgb2 = GPU_PREVIOUS;
			break;
		case GL_TEXTURE:
			//TOTDO: should this be the current texture???
			ctr_state.texenv_src_rgb2 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE0:
			ctr_state.texenv_src_rgb2 = GPU_TEXTURE0;
			break;
		case GL_TEXTURE1:
			ctr_state.texenv_src_rgb2 = GPU_TEXTURE1;
			break;
		case GL_TEXTURE2:
			ctr_state.texenv_src_rgb2 = GPU_TEXTURE2;
			break;
		}
		break;
	}
	ctr_state.dirty_texenv_src = 1;
}


void glTexEnvf(GLenum target, GLenum pname, GLfloat param) {
	switch (target) {
	case GL_TEXTURE_ENV:
		switch (pname) {
		case GL_SRC0_RGB:
		case GL_SRC1_RGB:
		case GL_SRC2_RGB:
			ctr_rend_texenv_src_rgb(pname, (u32)param);
			break;
		case GL_SRC0_ALPHA:
		case GL_SRC1_ALPHA:
		case GL_SRC2_ALPHA:
			ctr_rend_texenv_src_alpha(pname, (u32)param);
			break;
		case GL_OPERAND0_RGB:
		case GL_OPERAND1_RGB:
		case GL_OPERAND2_RGB:
			ctr_rend_texenv_op_rgb(pname, (u32)param);
			break;
		case GL_OPERAND0_ALPHA:
		case GL_OPERAND1_ALPHA:
		case GL_OPERAND2_ALPHA:
			ctr_rend_texenv_op_alpha(pname, (u32)param);
			break;
		case GL_COMBINE_RGB:
			ctr_rend_texenv_comb_rgb((u32)param);
			break;
		case GL_COMBINE_ALPHA:
			ctr_rend_texenv_comb_alpha((u32)param);
			break;
		}
	}
}

void glTexEnvi(GLenum target, GLenum pname, GLint param) {
	switch (target) {
	case GL_TEXTURE_ENV:
		switch (pname) {
		case GL_SRC0_RGB:
		case GL_SRC1_RGB:
		case GL_SRC2_RGB:
			ctr_rend_texenv_src_rgb(pname, (u32)param);
			break;
		case GL_SRC0_ALPHA:
		case GL_SRC1_ALPHA:
		case GL_SRC2_ALPHA:
			ctr_rend_texenv_src_alpha(pname, (u32)param);
			break;
		case GL_OPERAND0_RGB:
		case GL_OPERAND1_RGB:
		case GL_OPERAND2_RGB:
			ctr_rend_texenv_op_rgb(pname, (u32)param);
			break;
		case GL_OPERAND0_ALPHA:
		case GL_OPERAND1_ALPHA:
		case GL_OPERAND2_ALPHA:
			ctr_rend_texenv_op_alpha(pname, (u32)param);
			break;
		case GL_COMBINE_RGB:
			ctr_rend_texenv_comb_rgb((u32)param);
			break;
		case GL_COMBINE_ALPHA:
			ctr_rend_texenv_comb_alpha((u32)param);
			break;
		}
	}
}

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	int ir = 255 * r;
	int ig = 255 * g;
	int ib = 255 * b;
	int ia = 255 * a;

	if (ir > 255) ir = 255;
	if (ig > 255) ig = 255;
	if (ib > 255) ib = 255;
	if (ia > 255) ia = 255;
	if (ir < 0) ir = 0;
	if (ig < 0) ig = 0;
	if (ib < 0) ib = 0;
	if (ia < 0) ia = 0;

	ctr_state.texenv_color_r = ir;
	ctr_state.texenv_color_g = ig;
	ctr_state.texenv_color_b = ib;
	ctr_state.texenv_color_a = ia;
	ctr_state.dirty_texenv_color = 1;
}
