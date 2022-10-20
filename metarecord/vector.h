#ifndef  APPLICATION_VECTOR_H
# define APPLICATION_VECTOR_H

# include <QVector>
# include "metarecordable.h"

template<typename VALUE_TYPE, typename SOURCE>
QVector<VALUE_TYPE*> model_vector_cast(const SOURCE& source)
{
  QVector<VALUE_TYPE*> result;

  result.reserve(source.size());
  for (MetaRecordable* model : source)
    result.push_back(reinterpret_cast<VALUE_TYPE*>(model));
  return result;
}

template<typename LIST>
LIST select(const LIST& source, std::function<bool (typename LIST::value_type)> condition)
{
  LIST list;

  for (auto it = source.begin() ; it != source.end() ; ++it)
  {
    if (condition(*it))
      list.push_back(*it);
  }
  return list;
}

#endif
