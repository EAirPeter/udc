#ifndef UDC_COMMON_HPP_
#define UDC_COMMON_HPP_

#define CAT_IMPL(x_, y_) x_ ## y_
#define CAT(x_, y_) CAT_IMPL(x_, y_)
#define UNI_NAME CAT(vUniqueName_, __COUNTER__)

#endif
