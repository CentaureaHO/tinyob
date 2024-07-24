#include "message.h"
#include <cereal/types/polymorphic.hpp>

/**
 * @brief 注册类型
 *
 * 使用Cereal库注册SqlCommand、SqlResult、SqlExecuteResult和SqlQueryResult类型，
 * 以便进行多态序列化和反序列化。
 */
CEREAL_REGISTER_TYPE(SqlCommand)
CEREAL_REGISTER_TYPE(SqlResult)
CEREAL_REGISTER_TYPE(SqlExecuteResult)
CEREAL_REGISTER_TYPE(SqlQueryResult)

/**
 * @brief 注册多态关系
 *
 * 注册Message与SqlCommand、SqlResult的多态关系，
 * 以及SqlResult与SqlExecuteResult、SqlQueryResult的多态关系，
 * 以便Cereal库在序列化和反序列化时能够正确处理继承关系。
 */
CEREAL_REGISTER_POLYMORPHIC_RELATION(Message, SqlCommand)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Message, SqlResult)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SqlResult, SqlExecuteResult)
CEREAL_REGISTER_POLYMORPHIC_RELATION(SqlResult, SqlQueryResult)
