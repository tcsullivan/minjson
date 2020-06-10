#ifndef MINJSON_TYPE_HPP_
#define MINJSON_TYPE_HPP_

namespace minjson
{
    /**
     * Enumerates the different possible types of 'objects' within JSON.
     */
    enum class type
    {
        string,
        number,
        object,
        array,
        boolean,
        null
    };
}

#endif // MINJSON_TYPE_HPP_

