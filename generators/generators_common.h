/*
 * generators_common.h
 *
 * Tool used to parse an input template .h and .cpp files
 * and replace tokens for output files based on parameters.
 *
 * See LICENCE.md for Copyright information.
 */
#ifndef YIQI_GENERATORS_COMMON_H
#define YIQI_GENERATORS_COMMON_H

#include <functional>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

namespace yiqi
{
    namespace generators
    {
        void
        GetOptions (int                       argc,
                    char                      **argv,
                    std::string               &input,
                    std::string               &output,
                    std::vector <std::string> &tools);

        std::string
        ReadInputFile (std::string const &input);

        void
        WriteToOutputFile (std::string const &output,
                           std::string const &data);

        template <typename CurrentTransform>
        void
        ApplyTransformations (std::string            &data,
                              CurrentTransform const &transform)
        {
            transform (data);
        }

        template <typename CurrentTransform, typename... Transformations>
        void
        ApplyTransformations (std::string            &data,
                              CurrentTransform const &transform,
                              Transformations...     transformations)
        {
            transform (data);
            ApplyTransformations (data, transformations...);
        }

        namespace util
        {
            unsigned int
            IndentationLevelOfToken (std::string &str, size_t pos);

            typedef std::vector <std::string> StringVector;

            void
            InsertStringsAtIndentationLevel (StringVector const &collection,
                                             unsigned int       indentation,
                                             size_t             insertPosition,
                                             size_t             replacementPos,
                                             std::string        &string);

            typedef std::function <std::string (std::string const &,
                                                size_t,
                                                size_t)> Transformation;

            void
            GenerateFromTools (std::string          &string,
                               std::string    const &searchToken,
                               Transformation const &transform,
                               StringVector   const &tools);
        }

        namespace transformations
        {
            void DeleteComments (std::string &str);
            void RemoveLineIfEndingWithDEL (std::string &str);
            void CompressNewlines (std::string &str);
            void GenerateInstrumentationToolsN (std::string &string,
                                                size_t toolsSize);
        }
    }
}

#endif
